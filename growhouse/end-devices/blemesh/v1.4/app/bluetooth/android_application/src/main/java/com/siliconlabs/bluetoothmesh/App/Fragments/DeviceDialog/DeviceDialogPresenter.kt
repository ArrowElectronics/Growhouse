/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.DeviceDialog

import android.app.AlertDialog
import android.content.DialogInterface
import com.siliconlab.bluetoothmesh.adk.data_model.node.Node
import com.siliconlabs.bluetoothmesh.App.BasePresenter
import com.siliconlabs.bluetoothmesh.App.Dialogs.DeviceEditionDialogsListener
import com.siliconlabs.bluetoothmesh.App.Fragments.DeviceDialog.Config.DeviceConfigListener
import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic
import com.siliconlabs.bluetoothmesh.App.Logic.NetworkConnectionLogic
import com.siliconlabs.bluetoothmesh.App.Models.DeviceConfig

/**
 * @author Comarch S.A.
 */
class DeviceDialogPresenter(private val deviceDialogView: DeviceDialogView, private val networkConnectionLogic: NetworkConnectionLogic, private val meshLogic: MeshLogic) : BasePresenter {

    var deviceConfigListener: DeviceConfigListener? = null

    var deviceEditionDialogsListener: DeviceEditionDialogsListener? = null

    val meshNode = meshLogic.deviceToConfigure!!

    override fun onResume() {
    }

    override fun onPause() {
    }

    private fun clearDeviceToConfig(){
        meshLogic.deviceToConfigure = null
    }

    fun onSaveDeviceConfigClicked(): Boolean {
        val deviceConfig = deviceConfigListener?.getDeviceConfig()
        deviceConfig?.apply {
            if (!validDeviceConfig(this)) {
                return false
            }

            if (disableProxyOnConnectedDevice(this, meshNode.node)) {
                deviceDialogView.showDisableProxyAttentionDialog(deviceConfig, DialogInterface.OnClickListener { _, which ->
                    when (which) {
                        AlertDialog.BUTTON_POSITIVE -> {
                            deviceEditionDialogsListener?.onDeviceConfigUpdated(meshNode, deviceConfig, true)
                        }
                    }
                })
                return false
            } else {
                deviceEditionDialogsListener?.onDeviceConfigUpdated(meshNode, deviceConfig)
            }
        }

        clearDeviceToConfig()
        return true
    }

    fun onCancelDeviceConfigClicked(){
        clearDeviceToConfig()
    }

    private fun disableProxyOnConnectedDevice(deviceConfig: DeviceConfig, node: Node): Boolean {
        val deviceInfo = networkConnectionLogic.getCurrentConnectedDeviceInfo()
        deviceInfo?.apply {
            if (deviceInfo != node) {
                return false
            }

            deviceConfig.proxy?.apply {
                if (!this) {
                    return true
                }
            }
        }
        return false
    }

    private fun validDeviceConfig(deviceConfig: DeviceConfig): Boolean {
        deviceConfig.name?.apply {
            if (trim().isEmpty()) {
                deviceConfigListener?.onDeviceNameErrorMessage(DeviceConfigListener.ERROR_MESSAGE.DEVICE_NAME_EMPTY)
                return false
            }
        }
        return true
    }

}