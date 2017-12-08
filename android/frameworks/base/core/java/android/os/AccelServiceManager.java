/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package android.os;

import android.os.AccelerometerSample;
import android.util.Log;

/**
 * Accel Service Manager
 */
public class AccelServiceManager {
	private static final String TAG = "AccelServiceManager";

	private final IAccelService mService;
	private final Binder mToken = new Binder();

	public AccelServiceManager() {
		mService = IAccelService.Stub.asInterface(
				ServiceManager.getService("accelservice"));
	}

	public int readAcceleration(AccelerometerSample data) {
		if (mService == null) {
			Log.w(TAG, "Failed to readAcceleration: no accel service.");
			return 0;
		}
		try {
            int ret;

			Log.i(TAG, "Calling accelservice readAcceleration");
			ret = mService.readAcceleration(data);
            Log.i(TAG, "x " + Double.toString(data.x) +
                       " y " + Double.toString(data.y) +
                       " z " + Double.toString(data.z));
            return ret;
		} catch (RemoteException e) {
			Log.e(TAG, "Calling accelservice readAcceleration failed: " + e.getMessage());
		}
		return 0;
	}

	public int setSampleRate(int samplesPerSecond) {
		if (mService == null) {
			Log.w(TAG, "Failed to setSampleRate; no accel service.");
			return 0;
		}
		try {
			Log.i(TAG, "Calling accelservice setSampleRate");
			return mService.setSampleRate(samplesPerSecond);
		} catch (RemoteException e) {
			Log.e(TAG, "Calling accelservice setSampleRate failed: " + e.getMessage());
		}
		return 0;
	}
}
