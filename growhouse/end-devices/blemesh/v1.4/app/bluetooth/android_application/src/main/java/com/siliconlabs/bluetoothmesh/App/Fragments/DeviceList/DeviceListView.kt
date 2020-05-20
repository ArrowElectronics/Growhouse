/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.DeviceList

import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlabs.bluetoothmesh.App.ModelView.MeshNode

/**
 * @author Comarch S.A.
 */
interface DeviceListView {

    fun setDevicesList(devicesInfo: Set<MeshNode>)

    fun notifyDataSetChanged()

    fun showLoadingDialog()

    fun dismissLoadingDialog()

    fun updateLoadingDialogMessage(loadingMessage: LOADING_DIALOG_MESSAGE, errorCode: String = "", showCloseButton: Boolean = false)

    fun updateLoadingDialogMessage(loadingMessage: LOADING_DIALOG_MESSAGE, errorType: ErrorType, showCloseButton: Boolean = false)

    fun showDeleteDeviceDialog(deviceInfo: List<MeshNode>)

    fun showDeviceConfigDialog(deviceInfo: MeshNode)

    fun dismissDeviceConfigDialog()

    fun showErrorToast(message: String)

    fun showErrorToast(errorType: ErrorType)

    fun showToast(toastMessage: TOAST_MESSAGE)

    fun startConfigDevice(deviceInfo: MeshNode)

    enum class LOADING_DIALOG_MESSAGE {
        CONFIG_CONNECTING_ERROR,
        CONFIG_CONNECTING,
        CONFIG_ERROR,
        CONFIG_SUCCESS,
        CONFIG_MISSING,
        CONFIG_NO_GROUP,
        CONFIG_ADDING_TO_GROUP,
        CONFIG_ADDED_TO_GROUP,
        CONFIG_REMOVING_FROM_GROUP,
        CONFIG_REMOVED_FROM_GROUP,
        CONFIG_DEVICE_DELETED,
        CONFIG_DEVICE_DELETING,
        CONFIG_PROXY_ENABLING,
        CONFIG_PROXY_DISABLING,
        CONFIG_PROXY_ENABLED,
        CONFIG_PROXY_DISABLED,
        CONFIG_PROXY_DELETED,
        CONFIG_PROXY_DISABLING_TIMEOUT,
        CONFIG_MODEL_ADDING,
        CONFIG_MODEL_REMOVING,
        CONFIG_MODEL_ADDED,
        CONFIG_MODEL_REMOVED,
        CONFIG_MODEL_INVALID_MODEL,
        CONFIG_SUBSCRIPTION_ADDING,
        CONFIG_SUBSCRIPTION_REMOVING,
        CONFIG_PUBLICATION_SETTING,
        CONFIG_PUBLICATION_CLEARING,
        CONFIG_FRIEND_ENABLING,
        CONFIG_FRIEND_ENABLED,
        CONFIG_FRIEND_DISABLING,
        CONFIG_FRIEND_DISABLED,
        CONFIG_CLIENT_IDENTITY_ENABLING,
        CONFIG_CLIENT_IDENTITY_DISABLING,
        CONFIG_RELAY_ENABLING,
        CONFIG_RELAY_DISABLING,
        CONFIG_RELAY_ENABLED,
        CONFIG_RELAY_DISABLED,
        CONFIG_REQUESTING_DCD,
        CONFIG_REQUESTING_DCD_TIMEOUT,
        CONFIG_REQUESTING_DCD_ERROR,
        CONFIG_REQUESTING_DCD_SUCCESS,
    }

    enum class TOAST_MESSAGE {
        ERROR_DELETE_DEVICE
    }

}