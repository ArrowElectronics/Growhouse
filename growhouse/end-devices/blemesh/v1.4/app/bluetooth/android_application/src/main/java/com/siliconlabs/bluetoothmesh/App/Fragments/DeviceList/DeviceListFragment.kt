/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.DeviceList

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ListView
import android.widget.Toast
import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlabs.bluetoothmesh.App.Activities.Main.MainActivity
import com.siliconlabs.bluetoothmesh.App.Dialogs.DeviceEditionDialogs
import com.siliconlabs.bluetoothmesh.App.Dialogs.DeviceEditionDialogsPresenter
import com.siliconlabs.bluetoothmesh.App.Fragments.Network.NetworkFragment
import com.siliconlabs.bluetoothmesh.App.Fragments.Network.NetworkView
import com.siliconlabs.bluetoothmesh.App.ModelView.MeshNode
import com.siliconlabs.bluetoothmesh.App.Utils.ErrorMessageConverter
import com.siliconlabs.bluetoothmesh.R
import dagger.android.support.DaggerFragment
import kotlinx.android.synthetic.main.devices_screen.*
import javax.inject.Inject


/**
 * @author Comarch S.A.
 */
class DeviceListFragment : DaggerFragment(), DeviceListView, DeviceEditionDialogs.ActivityProvider, DeviceEditionDialogsPresenter.ParentView {

    override fun startConfigDevice(deviceInfo: MeshNode) {
        deviceEditionDialogsPresenter.showDeviceConfigDialog(deviceInfo)
    }

    lateinit var deviceEditionDialogsPresenter: DeviceEditionDialogsPresenter
    lateinit var deviceEditionDialogs: DeviceEditionDialogs

    override fun showLoadingDialog() {
        deviceEditionDialogs.showLoadingDialog()
    }

    override fun dismissLoadingDialog() {
        deviceEditionDialogs.dismissLoadingDialog()
    }

    override fun updateLoadingDialogMessage(loadingMessage: DeviceListView.LOADING_DIALOG_MESSAGE, errorCode: String, showCloseButton: Boolean) {
        deviceEditionDialogs.updateLoadingDialogMessage(loadingMessage, errorCode, showCloseButton)
    }

    override fun updateLoadingDialogMessage(loadingMessage: DeviceListView.LOADING_DIALOG_MESSAGE, errorType: ErrorType, showCloseButton: Boolean) {
        context?.let {
            updateLoadingDialogMessage(loadingMessage, ErrorMessageConverter.convert(it, errorType), showCloseButton)
        }
    }

    private val TAG: String = javaClass.canonicalName

    @Inject
    lateinit var deviceListPresenter: DeviceListPresenter

    private var devicesListAdapter: DeviceListAdapter? = null

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        return inflater.inflate(R.layout.devices_screen, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        deviceEditionDialogs = DeviceEditionDialogs(this, deviceListPresenter.meshLogic)
        deviceEditionDialogsPresenter = DeviceEditionDialogsPresenter(deviceEditionDialogs, this, deviceListPresenter.meshLogic, deviceListPresenter.meshNodeManager)
        deviceEditionDialogs.deviceEditionDialogsListener = deviceEditionDialogsPresenter

        devicesListAdapter = DeviceListAdapter(context!!, deviceListPresenter.networkConnectionLogic, deviceListPresenter)
        devices_list.adapter = devicesListAdapter
        devices_list.choiceMode = ListView.CHOICE_MODE_MULTIPLE_MODAL
        devices_list.setMultiChoiceModeListener(devicesListAdapter)
        devices_list.emptyView = ll_empty_view

        devices_list.setOnItemClickListener { _, view, position, _ ->
            devicesListAdapter?.onItemClickListener(view)
        }
    }

    override fun onResume() {
        super.onResume()
        deviceListPresenter.onResume()
    }

    override fun onPause() {
        super.onPause()
        deviceListPresenter.onPause()
        devicesListAdapter?.finishActionMode()
    }

    override fun setUserVisibleHint(isVisibleToUser: Boolean) {
        super.setUserVisibleHint(isVisibleToUser)
        if (!isVisibleToUser) {
            devicesListAdapter?.finishActionMode()
        }
    }

    // ParentView

    override fun refreshList() {
        activity?.runOnUiThread {
            deviceListPresenter.refreshList()
            (parentFragment as NetworkFragment).refreshFragment(NetworkView.FRAGMENT.GROUP_LIST)
        }
    }

    override fun returnToNetworkList() {
        val mainActivity = activity as MainActivity
        mainActivity.returnToNetworkListFragment()
    }

    override fun showToast(message: DeviceEditionDialogsPresenter.TOAST_MESSAGE) {
        activity?.runOnUiThread {
            val stringMessage: String? = when (message) {
                DeviceEditionDialogsPresenter.TOAST_MESSAGE.ERROR_DELETE_DEVICE -> getString(R.string.device_adapter_remove_device_error)
                DeviceEditionDialogsPresenter.TOAST_MESSAGE.ERROR_MISSING_GROUP -> "Missing group"
            }

            stringMessage?.let {
                Toast.makeText(activity, stringMessage, Toast.LENGTH_SHORT).show()
            }
        }
    }

    //

    fun refreshView() {
        devicesListAdapter?.notifyDataSetChanged()
    }

    override fun setDevicesList(newDevicesList: Set<MeshNode>) {
        activity?.runOnUiThread {
            devicesListAdapter?.clear()
            devicesListAdapter?.addAll(newDevicesList.toMutableList())
            devicesListAdapter?.notifyDataSetChanged()
        }
    }

    override fun notifyDataSetChanged() {
        activity?.runOnUiThread {
            devicesListAdapter?.notifyDataSetChanged()
        }
    }

    override fun showDeleteDeviceDialog(devicesInfo: List<MeshNode>) {
        deviceEditionDialogs.showDeleteDeviceDialog(devicesInfo)
        devicesListAdapter?.finishActionMode()
    }

    override fun showDeviceConfigDialog(deviceInfo: MeshNode) {
        startConfigDevice(deviceInfo)
        devicesListAdapter?.finishActionMode()
    }

    override fun dismissDeviceConfigDialog() {
    }

    override fun showErrorToast(message: String) {
        activity?.runOnUiThread {
            Toast.makeText(activity, message, Toast.LENGTH_LONG).show()
        }
    }

    override fun showErrorToast(errorType: ErrorType) {
        showErrorToast(ErrorMessageConverter.convert(activity!!, errorType))
    }

    override fun showToast(toastMessage: DeviceListView.TOAST_MESSAGE) {
        activity?.runOnUiThread {
            val message: String? = when (toastMessage) {
                DeviceListView.TOAST_MESSAGE.ERROR_DELETE_DEVICE -> getString(R.string.device_adapter_remove_device_error)
            }

            message?.let {
                Toast.makeText(activity, message, Toast.LENGTH_SHORT).show()
            }
        }
    }

}
