/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.DeviceDialog.Info

import com.siliconlabs.bluetoothmesh.App.ModelView.MeshNode

/**
 * @author Comarch S.A.
 */
interface DeviceInfoView {

    fun setDeviceInfo(deviceInfo: MeshNode)
}