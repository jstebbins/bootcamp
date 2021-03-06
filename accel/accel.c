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
#include <linux/i2c.h>
#include "lis331dlh_regs.h"

#define DRV_VERSION "0.01"

struct accel_device_data {
};

struct i2c_client * global_client;

static ssize_t accel_x_show(struct device *dev,
                            struct device_attribute *attr, char *buf)
{
    struct i2c_client * client;
    s32 lo, hi;

    client = dev_get_platdata(dev);
    if (client == NULL) {
        printk("client == NULL!\n");
        return 0;
    }
    lo = i2c_smbus_read_byte_data(client, LIS331DLH_OUT_X_L_REG);
    hi = i2c_smbus_read_byte_data(client, LIS331DLH_OUT_X_H_REG);
    snprintf(buf, PAGE_SIZE, "%d\n", (s16)((hi << 8) + lo));
    return strlen(buf) + 1;
}

static ssize_t accel_y_show(struct device *dev,
                            struct device_attribute *attr, char *buf)
{
    struct i2c_client * client;
    s32 lo, hi;

    client = dev_get_platdata(dev);
    if (client == NULL) {
        printk("client == NULL!\n");
        return 0;
    }
    lo = i2c_smbus_read_byte_data(client, LIS331DLH_OUT_Y_L_REG);
    hi = i2c_smbus_read_byte_data(client, LIS331DLH_OUT_Y_H_REG);
    snprintf(buf, PAGE_SIZE, "%d\n", (s16)((hi << 8) + lo));
    return strlen(buf) + 1;
}

static ssize_t accel_z_show(struct device *dev,
                            struct device_attribute *attr, char *buf)
{
    struct i2c_client * client;
    s32 lo, hi;

    client = dev_get_platdata(dev);
    if (client == NULL) {
        printk("client == NULL!\n");
        return 0;
    }
    lo = i2c_smbus_read_byte_data(client, LIS331DLH_OUT_Z_L_REG);
    hi = i2c_smbus_read_byte_data(client, LIS331DLH_OUT_Z_H_REG);
    snprintf(buf, PAGE_SIZE, "%d\n", (s16)((hi << 8) + lo));
    return strlen(buf) + 1;
}

static ssize_t accel_rate_show(struct device *dev,
                               struct device_attribute *attr, char *buf)
{
    struct i2c_client * client;
    s32 rate;

    client = dev_get_platdata(dev);
    if (client == NULL) {
        printk("client == NULL!\n");
        return 0;
    }
    rate = i2c_smbus_read_byte_data(client, LIS331DLH_CTRL1_REG);
    rate = (rate >> 3) & 0x3;
    switch (rate) {
        default:
        case 0:
            rate = 50;
            break;
        case 1:
            rate = 100;
            break;
        case 2:
            rate = 400;
            break;
        case 3:
            rate = 1000;
            break;
    }
    snprintf(buf, PAGE_SIZE, "%d\n", rate);
    return strlen(buf) + 1;
}

static void set_rate(struct i2c_client * client, int rate)
{
    int ret;

    if (rate <= 50)
        rate = 0;
    else if (rate <= 100)
        rate = 1;
    else if (rate <= 400)
        rate = 2;
    else
        rate = 3;
    ret = i2c_smbus_write_byte_data(client, LIS331DLH_CTRL1_REG,
                                    LIS331DLH_DEF_PM | (rate << 3));
    if (ret < 0) {
        printk("error: i2c_smbus_write_byte_data %d\n", ret);
    }
}

static ssize_t accel_rate_store(struct device *dev,
                                struct device_attribute *attr,
                                const char *buf, size_t count)
{
    struct i2c_client * client;
    int rate;

    printk("store got (%s)\n", buf);
    client = dev_get_platdata(dev);
    if (client == NULL) {
        printk("client == NULL!\n");
        return count;
    }

    sscanf(buf, "%d", &rate);
    set_rate(client, rate);
    return count;
}


static DEVICE_ATTR(x, 0444, accel_x_show, NULL);
static DEVICE_ATTR(y, 0444, accel_y_show, NULL);
static DEVICE_ATTR(z, 0444, accel_z_show, NULL);
static DEVICE_ATTR(rate, 0644, accel_rate_show, accel_rate_store);

// Do hardware init
static int accel_init(struct i2c_client * client,
                      struct accel_device_data *add)
{
    int ret;
    u32 who_am_i;

    ret = i2c_smbus_write_byte_data(client, LIS331DLH_CTRL1_REG,
                                            LIS331DLH_DEF_PM);
    if (ret < 0) {
        printk("error: i2c_smbus_write_byte_data %d\n", ret);
    }
    ret = i2c_smbus_write_byte_data(client, LIS331DLH_CTRL2_REG,
                                            LIS331DLH_REBOOT);
    if (ret < 0) {
        printk("error: i2c_smbus_write_byte_data %d\n", ret);
    }
    who_am_i = i2c_smbus_read_byte_data(client, LIS331DLH_WHO_AM_I_REG);
    printk("Who Am I %x\n", who_am_i);
    return 0;
}

static void accel_deinit(struct i2c_client *client,
                         struct accel_device_data *add)
{
}

static __devinit int accel_dev_probe(struct i2c_client *client,
                                     const struct i2c_device_id *id)
{
    int ret;
    struct accel_device_data *add;

    printk("accel_dev_probe\n");
    add = kmalloc(sizeof(struct accel_device_data), GFP_KERNEL);
    if (add == NULL) {
        printk("accel_dev_proble kmalloc failed\n");
        return -ENODEV;
    }
    memset(add, 0, sizeof(struct accel_device_data));

    i2c_set_clientdata(client, add);

    client->dev.platform_data = client;
    ret = device_create_file(&client->dev, &dev_attr_x);
    if (ret < 0)
        printk("device_create_file x FAILED!\n");
    ret = device_create_file(&client->dev, &dev_attr_y);
    if (ret < 0)
        printk("device_create_file y FAILED!\n");
    ret = device_create_file(&client->dev, &dev_attr_z);
    if (ret < 0)
        printk("device_create_file z FAILED!\n");
    ret = device_create_file(&client->dev, &dev_attr_rate);
    if (ret < 0)
        printk("device_create_file rate FAILED!\n");

    // Do hardware init
    return accel_init(client, add);
}

static __devexit int accel_dev_remove(struct i2c_client * client)
{
    struct accel_device_data *add = i2c_get_clientdata(client);
    printk("accel_dev_remove\n");

    // Deinit hardware
    accel_deinit(client, add);
    device_remove_file(&client->dev, &dev_attr_x);
    device_remove_file(&client->dev, &dev_attr_y);
    device_remove_file(&client->dev, &dev_attr_z);
    device_remove_file(&client->dev, &dev_attr_rate);
    kfree(add);
    return 0;
}

static const struct i2c_device_id accel_id[] = {
    { "accelerometer", 0 },
    { },
};
MODULE_DEVICE_TABLE(i2c, accel_id);

static struct i2c_driver accel_dev_driver = {
    .driver = {
        .name = "accelerometer",
        .owner = THIS_MODULE,
    },
    .id_table = accel_id,
    .probe = accel_dev_probe,
    .remove = accel_dev_remove,
};

static int __init accel_dev_init(void)
{
    struct i2c_adapter    * adapter;
    struct i2c_board_info   info = {};

    printk("accel_dev_init\n");
    adapter = i2c_get_adapter(1);
    if (adapter == NULL) {
        printk("i2c_get_adaptter failed\n");
        return -ENODEV;
    }

    strlcpy(info.type, "accelerometer", I2C_NAME_SIZE);
    info.addr = LIS331DLH_I2C_ADDR;

    global_client = i2c_new_device(adapter, &info);
    if (global_client == NULL) {
        printk("i2c_new_device failed\n");
        return -ENODEV;
    }

    return i2c_add_driver(&accel_dev_driver);
}

static void __exit accel_dev_exit(void)
{
    printk("accel_dev_exit\n");
    i2c_unregister_device(global_client);
    i2c_del_driver(&accel_dev_driver);
}

MODULE_AUTHOR("John Stebbins <stebbins@jetheaddev.com>");
MODULE_DESCRIPTION("Bootcamp accel");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

module_init(accel_dev_init);
module_exit(accel_dev_exit);
