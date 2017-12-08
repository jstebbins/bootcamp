/*
 * Copyright (C) 2008 The Android Open Source Project
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

package com.android.server;

import java.io.*;

import android.content.Context;
import android.os.IAccelService;
import android.util.Slog;
import android.os.AccelerometerSample;

public class AccelService extends IAccelService.Stub {
	private static final String TAG = "AccelService";

	AccelService() {
		Slog.i(TAG, "AccelService\n");
	}

	public int readAcceleration(AccelerometerSample data) {
		data.x = getAcceleration("x");
		data.y = getAcceleration("y");
		data.z = getAcceleration("z");
		return 0;
	}
	
	public double getAcceleration(String attr) {
		String sysfs = new String("sys/bus/i2c/devices/1-0018/" + attr);
		File file = new File(sysfs);
		BufferedReader reader = null;
		double value = 0.0;

		try {
			reader = new BufferedReader(new FileReader(file));
			String text = null;

			if ((text = reader.readLine()) != null) {
				value = Integer.parseInt(text) / 32768.0 * 2.0;
			}
		} catch (FileNotFoundException e) {
			Slog.e(TAG, "File not found");
		} catch (IOException e) {
			Slog.e(TAG, "Read error");
		} finally {
			try {
				if (reader != null) {
					reader.close();
				}
			} catch (IOException e) {
			}
		}
		
		Slog.i(TAG, "readAcceleration\n");

		return value;
	}

	public int setSampleRate(int samplesPerSecond) {
		String sysfs = new String("sys/bus/i2c/devices/1-0018/rate");
		File file = new File(sysfs);
		BufferedWriter writer = null;

		try {
			writer = new BufferedWriter(new FileWriter(file));
			String text = Integer.toString(samplesPerSecond);

			writer.write(text, 0, text.length());
		} catch (FileNotFoundException e) {
			Slog.e(TAG, "File not found");
		} catch (IOException e) {
			Slog.e(TAG, "Write error");
		} finally {
			try {
				if (writer != null) {
					writer.close();
				}
			} catch (IOException e) {
			}
		}
		
		Slog.i(TAG, "setSampleRate to " + Integer.toString(samplesPerSecond) + "\n");

		return 0;
	}
}
