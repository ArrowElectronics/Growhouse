/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.DeviceDialog

import android.content.DialogInterface
import com.siliconlabs.bluetoothmesh.App.Models.DeviceConfig

/**
 * @author Comarch S.A.
 */
interface DeviceDialogView {

    fun showDisableProxyAttentionDialog(deviceConfig: DeviceConfig, clickListener: DialogInterface.OnClickListener)

}