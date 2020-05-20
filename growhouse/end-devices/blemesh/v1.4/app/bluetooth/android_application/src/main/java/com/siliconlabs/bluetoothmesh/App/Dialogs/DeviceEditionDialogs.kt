/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Dialogs

import android.app.Activity
import android.app.AlertDialog
import android.support.v4.app.FragmentManager
import android.view.LayoutInflater
import android.view.View
import android.view.WindowManager
import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlab.bluetoothmesh.adk.data_model.node.Node
import com.siliconlabs.bluetoothmesh.App.Fragments.DeviceDialog.DeviceDialogFragment
import com.siliconlabs.bluetoothmesh.App.Fragments.DeviceList.DeviceListView
import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic
import com.siliconlabs.bluetoothmesh.App.ModelView.MeshNode
import com.siliconlabs.bluetoothmesh.App.Models.DeviceConfig
import com.siliconlabs.bluetoothmesh.App.Utils.ErrorMessageConverter
import com.siliconlabs.bluetoothmesh.App.Views.CustomAlertDialogBuilder
import com.siliconlabs.bluetoothmesh.R
import kotlinx.android.synthetic.main.dialog_loading.*

class DeviceEditionDialogs(val activityProvider: ActivityProvider, private val meshLogic: MeshLogic) {
    private var loadingDialog: AlertDialog? = null
    lateinit var deviceEditionDialogsListener: DeviceEditionDialogsListener

    interface ActivityProvider {
        fun getActivity(): Activity?
        fun getChildFragmentManager(): FragmentManager
    }

    fun showDeleteDeviceDialog(devicesInfo: List<MeshNode>) {
        activityProvider.getActivity()?.runOnUiThread {
            val builder = AlertDialog.Builder(activityProvider.getActivity(), R.style.AppTheme_Light_Dialog_Alert)
            builder.apply {

                val proxy = devicesInfo.find { device -> device.node.isConnectedAsProxy }
                if (proxy != null) {
                    setMessage(R.string.devices_dialog_delete_from_proxy_message)
                }

                setTitle(context.getString(R.string.devices_dialog_delete_title))

                setPositiveButton(context.getString(R.string.dialog_positive_delete)) { dialog, _ ->
                    deviceEditionDialogsListener.deleteDevices(devicesInfo)
                    dialog.dismiss()
                }
                setNegativeButton(R.string.dialog_negative_cancel) { dialog, _ ->
                    dialog.dismiss()
                }
            }

            val dialog = builder.create()
            dialog.apply {
                show()
            }
        }
    }

    fun showDeleteDeviceLocallyDialog(errorType: ErrorType, node: Node) {
        activityProvider.getActivity()?.runOnUiThread {
            val builder = AlertDialog.Builder(activityProvider.getActivity(), R.style.AppTheme_Light_Dialog_Alert)
            builder.apply {
                setTitle(R.string.device_dialog_delete_locally_title)

                setMessage(context.getString(R.string.device_dialog_delete_locally_message, ErrorMessageConverter.convert(activityProvider.getActivity()!!, errorType), node.name))

                setPositiveButton(context.getString(R.string.dialog_positive_delete)) { dialog, _ ->
                    deviceEditionDialogsListener.deleteDeviceLocally(node)
                    dialog.dismiss()
                }

                setNegativeButton(R.string.dialog_negative_cancel) { dialog, _ ->
                    dialog.dismiss()
                }
            }

            builder.create().show()
        }
    }

    fun showDeviceConfigDialog(deviceInfo: MeshNode) {
        activityProvider.getActivity()?.runOnUiThread {
            val childFragmentManager = activityProvider.getChildFragmentManager()
            val ft = childFragmentManager.beginTransaction()
            val prev = childFragmentManager.findFragmentByTag("dialog")
            if (prev != null) {
                ft.remove(prev)
            }

            ft.addToBackStack(null)


            meshLogic.deviceToConfigure = deviceInfo
            val dialog = DeviceDialogFragment()
            dialog.show(childFragmentManager, "dialog")

            dialog.deviceEditionDialogsListener = object : com.siliconlabs.bluetoothmesh.App.Dialogs.DeviceEditionDialogsListener {
                override fun onDeviceConfigUpdated(meshNode: MeshNode, deviceConfig: DeviceConfig, exitFromNetworkOnFinish: Boolean) {
                    deviceEditionDialogsListener.updateDeviceConfig(meshNode, deviceConfig, exitFromNetworkOnFinish)
                }
            }
        }
    }

    fun showLoadingDialog() {
        activityProvider.getActivity()?.runOnUiThread {
            loadingDialog?.apply {
                if (isShowing) {
                    return@runOnUiThread
                }
            }

            val view: View = LayoutInflater.from(activityProvider.getActivity()).inflate(R.layout.dialog_loading, null)
            val builder = CustomAlertDialogBuilder(activityProvider.getActivity()!!, R.style.AppTheme_Light_Dialog_Alert_Wrap)
            builder.apply {
                setView(view)
                setCancelable(false)
                setPositiveButton(activityProvider.getActivity()!!.getString(R.string.dialog_positive_ok)) { dialog, _ ->
                }
            }

            loadingDialog = builder.create()
            loadingDialog?.apply {
                window.setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE)
                show()

                getButton(AlertDialog.BUTTON_POSITIVE).visibility = View.GONE
            }
        }
    }

    fun dismissLoadingDialog() {
        activityProvider.getActivity()?.runOnUiThread {
            loadingDialog?.apply {
                dismiss()
                deviceEditionDialogsListener.dismiss()
            }
            loadingDialog = null
        }
    }

    fun updateLoadingDialogMessage(loadingMessage: DeviceListView.LOADING_DIALOG_MESSAGE, message: String = "", showCloseButton: Boolean = false) {
        val activity = activityProvider.getActivity()!!

        when (loadingMessage) {
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_CONNECTING_ERROR -> updateLoadingDialogMessage(activity.getString(R.string.device_config_connecting_error).format(message), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_CONNECTING -> updateLoadingDialogMessage(activity.getString(R.string.device_config_connecting), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_ERROR -> updateLoadingDialogMessage(activity.getString(R.string.device_config_error).format(message), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_SUCCESS -> updateLoadingDialogMessage(activity.getString(R.string.device_config_success), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_MISSING -> updateLoadingDialogMessage(activity.getString(R.string.device_config_missing), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_NO_GROUP -> updateLoadingDialogMessage(activity.getString(R.string.device_config_no_group), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_ADDING_TO_GROUP -> updateLoadingDialogMessage(activity.getString(R.string.device_config_adding_to_group).format(message), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_REMOVING_FROM_GROUP -> updateLoadingDialogMessage(activity.getString(R.string.device_config_removing_from_group).format(message), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_DEVICE_DELETED -> updateLoadingDialogMessage(activity.getString(R.string.device_config_device_deleted), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_DEVICE_DELETING -> updateLoadingDialogMessage(activity.getString(R.string.device_config_device_deleting), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_PROXY_ENABLING -> updateLoadingDialogMessage(activity.getString(R.string.device_config_proxy_enabling), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_PROXY_DISABLING -> updateLoadingDialogMessage(activity.getString(R.string.device_config_proxy_disabling), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_PROXY_ENABLED -> updateLoadingDialogMessage(activity.getString(R.string.device_config_proxy_enabled), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_PROXY_DISABLED -> updateLoadingDialogMessage(activity.getString(R.string.device_config_proxy_disabled), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_PROXY_DELETED -> updateLoadingDialogMessage(activity.getString(R.string.device_config_proxy_deleted), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_PROXY_DISABLING_TIMEOUT -> updateLoadingDialogMessage(activity.getString(R.string.device_config_proxy_disabling_timeout), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_MODEL_ADDING -> updateLoadingDialogMessage(activity.getString(R.string.device_config_model_adding).format(message), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_MODEL_REMOVING -> updateLoadingDialogMessage(activity.getString(R.string.device_config_model_removing).format(message), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_MODEL_ADDED -> updateLoadingDialogMessage(activity.getString(R.string.device_config_model_added).format(message), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_MODEL_REMOVED -> updateLoadingDialogMessage(activity.getString(R.string.device_config_model_removed).format(message), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_MODEL_INVALID_MODEL -> updateLoadingDialogMessage(activity.getString(R.string.device_config_model_invalid_model).format(message), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_SUBSCRIPTION_ADDING -> updateLoadingDialogMessage(activity.getString(R.string.device_config_subscription_adding).format(message), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_SUBSCRIPTION_REMOVING -> updateLoadingDialogMessage(activity.getString(R.string.device_config_subscription_removing).format(message), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_PUBLICATION_SETTING -> updateLoadingDialogMessage(activity.getString(R.string.device_config_publication_setting).format(message), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_PUBLICATION_CLEARING -> updateLoadingDialogMessage(activity.getString(R.string.device_config_publication_clearing).format(message), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_FRIEND_ENABLING -> updateLoadingDialogMessage(activity.getString(R.string.device_config_friend_enabling), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_FRIEND_DISABLING -> updateLoadingDialogMessage(activity.getString(R.string.device_config_friend_disabling), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_CLIENT_IDENTITY_ENABLING -> updateLoadingDialogMessage(activity.getString(R.string.device_config_client_identity_enabling), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_CLIENT_IDENTITY_DISABLING -> updateLoadingDialogMessage(activity.getString(R.string.device_config_client_identity_disabling), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_RELAY_ENABLING -> updateLoadingDialogMessage(activity.getString(R.string.device_config_relay_enabling), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_RELAY_DISABLING -> updateLoadingDialogMessage(activity.getString(R.string.device_config_relay_disabling), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_RELAY_ENABLED -> updateLoadingDialogMessage(activity.getString(R.string.device_config_relay_enabled), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_RELAY_DISABLED -> updateLoadingDialogMessage(activity.getString(R.string.device_config_relay_disabled), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_REQUESTING_DCD -> updateLoadingDialogMessage(activity.getString(R.string.device_config_dcd_requesting), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_REQUESTING_DCD_TIMEOUT -> updateLoadingDialogMessage(activity.getString(R.string.device_config_dcd_timeout_requesting), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_REQUESTING_DCD_ERROR -> updateLoadingDialogMessage(activity.getString(R.string.device_config_dcd_error_requesting).format(message), showCloseButton)
            DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_REQUESTING_DCD_SUCCESS -> updateLoadingDialogMessage(activity.getString(R.string.device_config_dcd_success_requesting), showCloseButton)
            else -> updateLoadingDialogMessage("TODO", showCloseButton) //TODO
        }
    }

    fun updateLoadingDialogMessage(errorType: ErrorType) {
        updateLoadingDialogMessage(ErrorMessageConverter.convert(activityProvider.getActivity()!!, errorType), true)
    }

    private fun updateLoadingDialogMessage(message: String = "", showCloseButton: Boolean = false) {
        activityProvider.getActivity()?.runOnUiThread {
            loadingDialog?.apply {
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

    interface DeviceEditionDialogsListener {
        fun dismiss()
        fun deleteDevices(devicesInfo: List<MeshNode>)
        fun deleteDeviceLocally(node: Node)
        fun updateDeviceConfig(meshNode: MeshNode, deviceConfig: DeviceConfig, exitFromNetworkOnFinish: Boolean)
    }
}