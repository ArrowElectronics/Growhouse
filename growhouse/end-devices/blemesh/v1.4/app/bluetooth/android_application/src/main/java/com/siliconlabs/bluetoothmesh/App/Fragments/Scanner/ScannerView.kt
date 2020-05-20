/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.Scanner

import android.app.Activity
import com.siliconlabs.bluetoothmesh.App.Models.DeviceDescription

interface ScannerView {
    fun addDevice(deviceDescription: DeviceDescription)
    fun setScanMenuToActive(active: Boolean)
    fun getActivity(): Activity?
    fun clearAdapter()
    fun showToast(toastMessage: TOAST_MESSAGE)
    enum class TOAST_MESSAGE {
        NO_NETWORK_AVAILABLE

    }
}