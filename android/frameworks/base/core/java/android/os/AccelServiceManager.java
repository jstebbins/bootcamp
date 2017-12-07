/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package android.os;

import android.util.Log;

/**
 * Pi Service Manager
 */
public class PiServiceManager {
    private static final String TAG = "PiService";

    private final IPiService mService;
    private final Binder mToken = new Binder();

    public PiServiceManager() {
        mService = IPiService.Stub.asInterface(
                ServiceManager.getService("piservice"));
    }

    public double getPi() {
        if (mService == null) {
            Log.w(TAG, "Failed to getPi; no pi service.");
            return 0.0;
        }
        try {
	    Log.i(TAG, "Calling piservice getPi");
            return mService.getPi();
        } catch (RemoteException e) {
        }
        return 0.0;
    }
}
