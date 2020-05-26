/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.DeviceDialog.Config

import com.siliconlabs.bluetoothmesh.App.Models.DeviceConfig

/**
 * @author Comarch S.A.
 */
interface DeviceConfigListener {

    fun getDeviceConfig(): DeviceConfig

    fun onDeviceNameErrorMessage(message: ERROR_MESSAGE)

    enum class ERROR_MESSAGE{
        DEVICE_NAME_EMPTY
    }
}