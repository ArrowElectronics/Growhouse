/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Dialogs

import com.siliconlabs.bluetoothmesh.App.ModelView.MeshNode
import com.siliconlabs.bluetoothmesh.App.Models.DeviceConfig

/**
 * @author Comarch S.A.
 */
interface DeviceEditionDialogsListener {

    fun onDeviceConfigUpdated(meshNode: MeshNode, deviceConfig: DeviceConfig, exitFromNetworkOnFinish: Boolean = false)

}