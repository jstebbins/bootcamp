package com.example.accelerationtester;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.os.AccelServiceManager;
import android.os.AccelerometerSample;

public class MainActivity extends Activity {
	private static final String TAG = "AccelerationTester";
	
	private AccelerationMonitor accelMonitorThread = null;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		TextView accelValuesView = (TextView) findViewById(R.id.text_accel_values);
		accelValuesView.setText ("");
		
		EditText sampleRateView = (EditText) findViewById(R.id.text_sample_rate);
		sampleRateView.setText ("");
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	public class AccelerationMonitor extends Thread {
		private volatile boolean stop = false;
		
		public void run() {
			AccelServiceManager accelerometerManager = (AccelServiceManager) getSystemService ("AccelerometerManager");

			try {
				while (stop == false)
				{
					AccelerometerSample sample = new AccelerometerSample();
					int error = accelerometerManager.readAcceleration (sample);
					Log.v (TAG, String.format("readAcceleration error=%d x=%+1.6f  y=%+1.6f  z=%+1.6f", error, sample.x, sample.y, sample.z));
					sleep (20);
				}
			} catch (Exception e) {
				Log.v (TAG, "acclerationMonitor Exception: " + e);
			}
		}
		
		public void halt() {
			stop = true;
		}
	}
	
	public void readAcceleration(View view) {
		int error = 0;
		AccelerometerSample sample = new AccelerometerSample();
		Log.v (TAG, "readAcceleration");
	
		AccelServiceManager accelManager = (AccelServiceManager) getSystemService ("AccelerometerManager");

		Log.v (TAG, "readAcceleration accelManager " + accelManager);

        try {
		    error = accelManager.readAcceleration (sample);
        } catch (Exception e) {
            Log.v (TAG, "acclerationMonitor Exception: " + e);
        }

		Log.v (TAG, "readAcceleration error " + error);
	
		Log.v (TAG, "sample.x " + sample.x);
		Log.v (TAG, "sample.y " + sample.y);
		Log.v (TAG, "sample.z " + sample.z);
	
		Log.v (TAG, String.format("readAcceleration error=%d x=%+1.6f  y=%+1.6f  z=%+1.6f", error, sample.x, sample.y, sample.z));
		
		if (error == 0)
		{
			TextView accelValuesView = (TextView) findViewById(R.id.text_accel_values);
			accelValuesView.setText (String.format("x=%+1.6f  y=%+1.6f  z=%+1.6f", sample.x, sample.y, sample.z));
		}
	}
	
	public void startAccelerationMonitor(View view) {
		if (accelMonitorThread == null) {
			accelMonitorThread = new AccelerationMonitor ();
			accelMonitorThread.start();
		}
	}
	
	public void stopAccelerationMonitor(View view) {
		if (accelMonitorThread != null) {
			accelMonitorThread.halt ();
		}
	}

	public void setSampleRate(View view) {
		int error = 0;
		
		EditText sampleRateView = (EditText) findViewById(R.id.text_sample_rate);
		if (sampleRateView.length() > 0) {
			int samplesPerSecond = Integer.parseInt(sampleRateView.getText().toString());
					
			AccelServiceManager accelerometerManager = (AccelServiceManager) getSystemService ("AccelerometerManager");
			error = accelerometerManager.setSampleRate (samplesPerSecond);
			
			Log.v (TAG, String.format("setSampleRate error=%d sammplesPerSecond=%d", error, samplesPerSecond));
		}
	}
}
