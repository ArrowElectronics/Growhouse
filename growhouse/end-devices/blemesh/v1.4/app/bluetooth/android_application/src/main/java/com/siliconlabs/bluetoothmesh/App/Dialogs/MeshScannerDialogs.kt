/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Dialogs

import android.app.Activity
import android.app.AlertDialog
import android.content.DialogInterface
import android.view.LayoutInflater
import android.view.View
import android.view.WindowManager
import android.widget.AdapterView
import android.widget.BaseAdapter
import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlabs.bluetoothmesh.App.Fragments.Scanner.ScannerDialogsPresenter
import com.siliconlabs.bluetoothmesh.App.Utils.ErrorMessageConverter
import com.siliconlabs.bluetoothmesh.App.Views.CustomAlertDialogBuilder
import com.siliconlabs.bluetoothmesh.R
import kotlinx.android.synthetic.main.dialog_loading.*
import kotlinx.android.synthetic.main.dialog_provisioning_device.view.*

class MeshScannerDialogs(val scannerDialogsPresenter: ScannerDialogsPresenter) : MeshScannerDialogsView {
    private var provisionDialog: AlertDialog? = null
    private lateinit var networkAdapter: BaseAdapter
    private lateinit var loadingDialog: AlertDialog
    private lateinit var activity: Activity
    private val loadingTextMapping: HashMap<MeshScannerDialogsView.LoadingText, Int> = hashMapOf(
            MeshScannerDialogsView.LoadingText.DeviceProvisionedGattDisconnected to R.string.scanner_dialog_loading_text_device_provisioned_gatt_disconnected,
            MeshScannerDialogsView.LoadingText.DeviceSuccessfullyProvisioned to R.string.scanner_dialog_loading_text_device_successfully_provisioned,
            MeshScannerDialogsView.LoadingText.Provisioning to R.string.scanner_dialog_loading_text_provisioning,
            MeshScannerDialogsView.LoadingText.RedirectingToDevices to R.string.scanner_dialog_loading_text_redirecting_to_devices
    )

    override fun showProvisionDialog(defaultName: String) {
        activity.runOnUiThread {
            var view: View = LayoutInflater.from(activity).inflate(R.layout.dialog_provisioning_device, null)
            view.apply {
                device_name.setText(defaultName)
                device_name.setSelection(defaultName.length)
                sp_network.adapter = networkAdapter
                sp_network.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
                    override fun onItemSelected(parent: AdapterView<*>, view: View, position: Int, id: Long) {
                        scannerDialogsPresenter.onNetworkSelected(position)
                    }

                    override fun onNothingSelected(parent: AdapterView<*>) {
                        //nothing to do
                    }
                }
            }
            val builder = CustomAlertDialogBuilder(activity, R.style.AppTheme_Light_Dialog_Alert)
            builder.apply {
                setTitle(activity.getString(R.string.scanner_dialog_provisioning_device_title))
                setView(view)
                setDismissOnClickPositiveButton(false)
                setPositiveButton(activity.getString(R.string.dialog_positive_add)) { dialog, _ ->
                    if (scannerDialogsPresenter.validDeviceName(view.device_name.text.toString())) {
                        scannerDialogsPresenter.provisionClicked(view.device_name.text.toString())
                        dialog.dismiss()
                    } else {
                        view.device_name_text_input.error = activity.getString(R.string.scanner_dialog_empty_name)
                    }
                }
                setNegativeButton(R.string.dialog_negative_cancel) { dialog, _ ->
                    scannerDialogsPresenter.disconnectProvisioningClicked()
                    dialog.dismiss()
                }
            }

            provisionDialog = builder.create()
            provisionDialog?.apply {
                window.setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE)
                show()
            }
        }
    }

    override fun initLoadingDialog(activity: Activity) {
        this.activity = activity

        activity.runOnUiThread {
            var view: View = LayoutInflater.from(activity).inflate(R.layout.dialog_loading, null)
            val builder = CustomAlertDialogBuilder(activity, R.style.AppTheme_Light_Dialog_Alert_Wrap)
            builder.apply {
                setView(view)
                setCancelable(false)
                setPositiveButton(activity.getString(R.string.dialog_positive_ok)) { dialog, _ ->
                }
            }

            loadingDialog = builder.create()
        }
    }

    override fun showLoadingDialog() {
        activity.runOnUiThread {
            loadingDialog.apply {
                window.setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE)

                show()

                getButton(AlertDialog.BUTTON_POSITIVE).visibility = View.GONE

                loading_text.setText(R.string.scanner_dialog_loading_text)
                loading_icon.visibility = View.VISIBLE
            }
        }
    }

    override fun updateLoadingText(loadingText: MeshScannerDialogsView.LoadingText, status: String, showCloseButton: Boolean) {
        val loadingTextString = activity.getString(loadingTextMapping.get(loadingText)!!).format(status)
        updateLoadingText(loadingTextString, showCloseButton)
    }

    override fun updateLoadingText(errorType: ErrorType, showCloseButton: Boolean) {
        updateLoadingText(ErrorMessageConverter.convert(activity, errorType), showCloseButton)
    }

    private fun updateLoadingText(message: String = "", showCloseButton: Boolean = false) {
        activity.runOnUiThread {
            loadingDialog.apply {
                if (!isShowing) {
                    return@runOnUiThread
                }

                loading_text.text = message

                if (showCloseButton) {
                    loading_icon.visibility = View.GONE
                    getButton(AlertDialog.BUTTON_POSITIVE).visibility = View.VISIBLE
                }
            }
        }
    }

    override fun closeProvisionDialog() {
        activity.runOnUiThread {
            provisionDialog?.apply {
                if (isShowing) {
                    dismiss()
                }
            }
        }
    }

    override fun closeLoadingDialog() {
        activity.runOnUiThread {
            if (loadingDialog.isShowing) {
                loadingDialog.dismiss()
            }
        }
    }

    override fun setNetworkAdapter(networkAdapter: BaseAdapter) {
        this.networkAdapter = networkAdapter
    }

    override fun showDeviceAlreadyExistDialog(listener: DialogInterface.OnClickListener) {
        activity.runOnUiThread {
            val builder = CustomAlertDialogBuilder(activity, R.style.AppTheme_Light_Dialog_Alert)
            builder.setTitle(R.string.scanner_dialog_alert_title)
            builder.setMessage(R.string.scanner_dialog_alert_message)
            builder.setPositiveButton(activity.getString(R.string.dialog_positive_ok)) { dialog, which ->
                listener.onClick(dialog, which)
            }
            builder.setNegativeButton(activity.getString(R.string.dialog_negative_cancel)) { dialog, which ->
                listener.onClick(dialog, which)
            }
            builder.show()
        }
    }

    override fun isShowingLoading(): Boolean {
        return loadingDialog.isShowing
    }

    override fun isShowingProvisioning(): Boolean {
        if (provisionDialog == null) {
            return false
        }
        return provisionDialog!!.isShowing
    }
}
