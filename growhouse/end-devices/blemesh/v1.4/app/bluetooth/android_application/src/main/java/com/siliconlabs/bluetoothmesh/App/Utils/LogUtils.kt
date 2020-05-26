/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Utils

import android.content.pm.ApplicationInfo
import android.os.Environment
import java.io.File
import java.io.IOException
import java.util.*

/**
 * @author Comarch S.A.
 */
class LogUtils {
    companion object {
        fun setupLogcatLogs(appInfo: ApplicationInfo) {
            if (0 != appInfo.flags and ApplicationInfo.FLAG_DEBUGGABLE) {
                val f = File(Environment.getExternalStorageDirectory(), "ble_mesh_logs")
                if (!f.exists()) {
                    f.mkdirs()
                }

                val filePath = Environment.getExternalStorageDirectory().toString() + String.format("/ble_mesh_logs/logcat_%s.txt", Calendar.getInstance().time.toString())
                try {
                    Runtime.getRuntime().exec(arrayOf("logcat", "-v", "time", "-f", filePath))
                } catch (e: IOException) {
                }

            }
        }
    }
}