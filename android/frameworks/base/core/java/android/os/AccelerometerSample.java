
package android.os;

import android.util.Log;

public final class AccelerometerSample implements Parcelable {
	private static final String TAG = "AccelerometerSample";

    public static final Parcelable.Creator<AccelerometerSample> CREATOR
                            = new Parcelable.Creator<AccelerometerSample>() {
        public AccelerometerSample createFromParcel(Parcel in) {
            return new AccelerometerSample(in);
        }

        public AccelerometerSample[] newArray(int size) {
            return new AccelerometerSample[size];
        }
    };

	public double x;
	public double y;
	public double z;

    public AccelerometerSample() {
	    this.x = 0;
	    this.y = 0;
	    this.z = 0;
        Log.i(TAG, "AccelerometerSample() z " + Double.toString(this.z));
    }

    public AccelerometerSample(Parcel in){
        this.x = in.readDouble();
        this.y = in.readDouble();
        this.z = in.readDouble();
        Log.i(TAG, "AccelerometerSample(Parcel) z " + Double.toString(this.z));
    }

    public int describeContents() {
        return 0;
    }

    public void writeToParcel(Parcel dest, int flags) {
        dest.writeDouble(x);
        dest.writeDouble(y);
        dest.writeDouble(z);
        Log.i(TAG, "writeToParcel z " + Double.toString(z));
    }

    private void readFromParcel(Parcel source) {
        x = source.readDouble();
        y = source.readDouble();
        z = source.readDouble();
        Log.i(TAG, "readFromParcel z " + Double.toString(z));
    }
}
