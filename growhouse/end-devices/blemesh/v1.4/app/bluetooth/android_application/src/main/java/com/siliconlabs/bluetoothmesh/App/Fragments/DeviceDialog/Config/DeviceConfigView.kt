/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.DeviceDialog.Config

import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlab.bluetoothmesh.adk.data_model.group.Group
import com.siliconlabs.bluetoothmesh.App.ModelView.MeshNode
import com.siliconlabs.bluetoothmesh.App.Models.DeviceConfig

/**
 * @author Comarch S.A.
 */
interface DeviceConfigView {

    fun setDeviceConfig(deviceInfo: MeshNode, groupsInfo: DeviceConfig, groupsInSubnet: List<Group>)

    fun showLoadingDialog()

    fun setLoadingDialogMessage(message: String, showCloseButton: Boolean)

    fun setLoadingDialogMessage(errorType: ErrorType?, showCloseButton: Boolean)

    fun dismissLoadingDialog()
}