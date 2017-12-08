#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/a.out.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include "16c750_support.h"

#define DRV_VERSION "0.01"

#define UART2_BASE 0x48024000
#define UART2_LEN  0x1000

struct uart_device_data {
    void __iomem *mmio;
};

// Global copy of mmio needed for read/write_uart_reg_raw
void __iomem *uart2_mmio;

unsigned long bps_rate = 115200;

static void set_loopback(int loopback);

static int uart_open(struct inode *inode, struct file *file)
{
    printk("uart_open\n");
    return 0;
}

static ssize_t uart_read(struct file *file, char __user * buf,
                         size_t count, loff_t *ppos)
{
    char *kbuf;
    ssize_t ii, len;

    printk("uart_read\n");
    kbuf = kmalloc(count, GFP_KERNEL);
    len = read_uart_reg(UART_RXFIFO_LVL_REG);
    count = len < count ? len : count;
    for (ii = 0; ii < count; ii++) {
        kbuf[ii] = (char)read_uart_reg(UART_RHR_REG);
    }
    if (copy_to_user(buf, kbuf, count)) {
        kfree(kbuf);
        return -EFAULT;
    }
    kfree(kbuf);
    return count;
}

static ssize_t uart_write(struct file *file, const char __user * buf,
                          size_t count, loff_t *ppos)
{
    char *kbuf;
    size_t ii;

    printk("uart_write\n");
    kbuf = kmalloc(count, GFP_KERNEL);
    if (!kbuf)
        return -ENOMEM;

    if (copy_from_user(kbuf, buf, count)) {
        kfree(kbuf);
        return -EFAULT;
    }

    for (ii = 0; ii < count; ii++) {
        write_uart_reg(UART_THR_REG, kbuf[ii]);
    }
    kfree(kbuf);

    return count;
}

static int uart_release(struct inode *inode, struct file *file)
{
    printk("uart_release\n");
    return 0;
}

static const struct file_operations uart_device_fops = {
    .read = uart_read,
    .write = uart_write,
    //.poll = uart_fpoll,
    .open = uart_open,
    .release = uart_release,
};

static struct miscdevice uart_device = {
    MISC_DYNAMIC_MINOR, "uart_loop", &uart_device_fops
};


void write_uart_reg_raw(u32 reg_offset, u16 value)
{
    iowrite16(value, (char*)uart2_mmio + reg_offset);
}

u16 read_uart_reg_raw(u32 reg_offset)
{
    return ioread16((char*)uart2_mmio + reg_offset);
}

static ssize_t uart_loopback_show(struct device *dev,
                                  struct device_attribute *attr, char *buf)
{
    u16 value;
    int loop;

    value = read_uart_reg(UART_MCR_REG);
    loop = !!(value & 0x10);
    snprintf(buf, PAGE_SIZE, "%s\n", loop ? "On" : "Off");
    return strlen(buf) + 1;
}

static ssize_t uart_loopback_store(struct device *dev,
                                   struct device_attribute *attr,
                                   const char *buf, size_t count)
{
    printk("store got (%s)\n", buf);
    set_loopback(!strncasecmp(buf, "On", count < 2 ? count : 2));
    return count;
}

static ssize_t uart_rx_show(struct device *dev,
                            struct device_attribute *attr, char *buf)
{
    u16 value;
    value = read_uart_reg(UART_RHR_REG);
    snprintf(buf, PAGE_SIZE, "%c\n", (char)value);
    return strlen(buf) + 1;
}

static ssize_t uart_tx_store(struct device *dev,
                             struct device_attribute *attr,
                             const char *buf, size_t count)
{
    if (count <= 0)
        return count;
    write_uart_reg(UART_THR_REG, buf[0]);
    return count;
}

static DEVICE_ATTR(trx, 0644, uart_rx_show, uart_tx_store);
static DEVICE_ATTR(loopback, 0644, uart_loopback_show, uart_loopback_store);

static void set_loopback(int loopback)
{
    u16 value;
    int stop = 10;

    // Write 0x0002 to UART_SYSC_REG to reset the UART
    // Wait for bit 0 of UART_SYSS_REG to be set, indicating that the
    //   UART has reset correctly
    write_uart_reg(UART_SYSC_REG, 0x0002);
    while (stop) {
        value = read_uart_reg(UART_SYSS_REG);
        if (value & 1)
            break;
        mdelay(100);
        stop--;
    }
    printk("reset: stop %d\n", stop);
    if (!loopback)
        return;

    // Init loopback mode
    // Write 0x0010 to UART_MCR_REG
    // Write 0x0007 to UART_FCR_REG
    // Write 0x0003 to UART_LCR_REG
    // Write 0x0001 to UART_DLH_REG
    // Write 0x0017 to UART_MCR_REG
    // Write 0x0000 to UART_MDR1_REG
    write_uart_reg(UART_MCR_REG, 0x0010);
    write_uart_reg(UART_FCR_REG, 0x0007);
    write_uart_reg(UART_LCR_REG, 0x0003);
    write_uart_reg(UART_DLH_REG, 0x0001);
    write_uart_reg(UART_MCR_REG, 0x0017);
    write_uart_reg(UART_MDR1_REG, 0x0000);

    // Verify MCR reg
    value = read_uart_reg(UART_MCR_REG);
    printk("Validate MCR %04x (s.b. 0x0017)\n", value);

    // Test writing to TX fifo
    write_uart_reg(UART_THR_REG, 'A');
    stop = 10;
    while (stop) {
        value = read_uart_reg(UART_RXFIFO_LVL_REG);
        if (value)
            break;
        mdelay(100);
        stop--;
    }
    printk("RX: stop %d\n", stop);
    value = read_uart_reg(UART_RHR_REG);
    printk("Read: (%c)\n", (char)value);
}

static int uart_init(struct platform_device * pd, struct uart_device_data *udd)
{
    int ret;
    uart2_mmio = udd->mmio = ioremap_nocache(UART2_BASE, UART2_LEN);
    if (!udd->mmio)
        return -EBUSY;
    init_uart_reg();
    set_loopback(1);

    uart_device.parent = &pd->dev;
    ret = misc_register(&uart_device);
    if (ret < 0) {
        pr_err("uart: error %d registering device\n", ret);
        return ret;
    }

    return 0;
}

static void uart_deinit(struct uart_device_data *udd)
{
    int ret;

    ret = misc_deregister(&uart_device);
    if (ret < 0) {
        pr_err("uart: error %d deregistering device\n", ret);
    }
    iounmap(udd->mmio);
}

static __devinit int uart_loop_probe(struct platform_device *pd)
{
    int ret;
    struct uart_device_data *udd;

    printk("uart_loop_probe\n");
    udd = kmalloc(sizeof(struct uart_device_data), GFP_KERNEL);
    memset(udd, 0, sizeof(struct uart_device_data));
    platform_set_drvdata(pd, udd);
    ret = device_create_file(&pd->dev, &dev_attr_loopback);
    if (ret < 0)
        printk("device_create_file loopback FAILED!\n");
    ret = device_create_file(&pd->dev, &dev_attr_trx);
    if (ret < 0)
        printk("device_create_file trx FAILED!\n");

    return uart_init(pd, udd);
}

static __devexit int uart_loop_remove(struct platform_device *pd)
{
    struct uart_device_data *udd = platform_get_drvdata(pd);
    printk("uart_loop_remove\n");
    uart_deinit(udd);
    device_remove_file(&pd->dev, &dev_attr_loopback);
    device_remove_file(&pd->dev, &dev_attr_trx);
    kfree(udd);
    return 0;
}

static struct platform_driver uart_loop_driver = {
    .probe = uart_loop_probe,
    .remove = __devexit_p(uart_loop_remove),
    .driver = {
        .owner = THIS_MODULE,
        .name = "barrometer_uart2",
    },
};

static int __init uart_loop_init(void)
{
    printk("uart_loop_init bps_rate %lu\n", bps_rate);
    return platform_driver_register(&uart_loop_driver);
}

static void __exit uart_loop_exit(void)
{
    printk("uart_loop_exit\n");
    platform_driver_unregister(&uart_loop_driver);
}

MODULE_AUTHOR("John Stebbins <stebbins@jetheaddev.com>");
MODULE_DESCRIPTION("Bootcamp loop uart");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

module_init(uart_loop_init);
module_exit(uart_loop_exit);
module_param(bps_rate, ulong, 0444);
