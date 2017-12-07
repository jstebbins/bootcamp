
package android.os;

import android.util.Log;

public class AccelerometerSample implements Parcelable {

    public static final Parcelable.Creator CREATOR = new Parcelable.Creator() {
        public AcceleromenterSample createFromParcel(Parcel in) {
            return new AcceleromenterSample(in);
        }

        public AcceleromenterSample[] newArray(int size) {
            return new AcceleromenterSample[size];
        }
    };

	public double x;
	public double y;
	public double z;

    public void writeToParcel(Parcel dest, int flags) {
        dest.writeDouble(x);
        dest.writeDouble(y);
        dest.writeDouble(z);
    }

    private void readFromParcel(Parcel source) {
        x = source.readDouble();
        y = source.readDouble();
        z = source.readDouble();
    }
}
