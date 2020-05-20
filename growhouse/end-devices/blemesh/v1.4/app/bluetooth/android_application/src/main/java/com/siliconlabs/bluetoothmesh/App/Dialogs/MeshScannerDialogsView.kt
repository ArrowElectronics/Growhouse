/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Dialogs

import android.app.Activity
import android.content.DialogInterface
import android.widget.BaseAdapter
import com.siliconlab.bluetoothmesh.adk.ErrorType

interface MeshScannerDialogsView {
    enum class LoadingText {
        Provisioning,

        DeviceProvisionedGattDisconnected,
        DeviceSuccessfullyProvisioned,
        RedirectingToDevices,
    }

    fun showDeviceAlreadyExistDialog(listener: DialogInterface.OnClickListener)

    fun setNetworkAdapter(netAdapter: BaseAdapter)

    fun closeLoadingDialog()

    fun closeProvisionDialog()

    fun showLoadingDialog()

    fun initLoadingDialog(activity: Activity)

    fun updateLoadingText(loadingText: LoadingText, status: String = "", showCloseButton: Boolean = false)

    fun updateLoadingText(errorType: ErrorType, showCloseButton: Boolean = false)

    fun showProvisionDialog(defaultName: String)

    fun isShowingLoading(): Boolean

    fun isShowingProvisioning(): Boolean

}