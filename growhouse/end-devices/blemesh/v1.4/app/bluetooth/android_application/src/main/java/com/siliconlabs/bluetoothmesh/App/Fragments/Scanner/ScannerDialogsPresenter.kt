/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.Scanner

import android.app.Activity
import android.app.AlertDialog
import android.content.DialogInterface
import android.util.Log
import android.widget.ArrayAdapter
import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlab.bluetoothmesh.adk.data_model.subnet.Subnet
import com.siliconlabs.bluetoothmesh.App.Activities.Main.MainActivity
import com.siliconlabs.bluetoothmesh.App.Dialogs.MeshScannerDialogs
import com.siliconlabs.bluetoothmesh.App.Dialogs.MeshScannerDialogsView
import com.siliconlabs.bluetoothmesh.App.Fragments.Network.NetworkFragment
import com.siliconlabs.bluetoothmesh.App.Models.DeviceDescription
import com.siliconlabs.bluetoothmesh.App.Models.ProvisioningModel
import com.siliconlabs.bluetoothmesh.App.Models.ProvisioningStatusPropagator
import com.siliconlabs.bluetoothmesh.R

class ScannerDialogsPresenter(val scannerPresenter: ScannerPresenter, val provisioningModel: ProvisioningModel) {
    private val TAG: String = javaClass.canonicalName

    private lateinit var activity: Activity
    private var spinner_value = 0
    val meshScannerDialogs: MeshScannerDialogsView = MeshScannerDialogs(this)

    internal fun init(activity: Activity) {
        this.activity = activity
        meshScannerDialogs.initLoadingDialog(activity)
    }

    private lateinit var networks: List<Subnet>

    fun configDevice(deviceDescription: DeviceDescription, networks: List<Subnet>) {
        if (!meshScannerDialogs.isShowingLoading()) {
            provisioningModel.selectedDevice = deviceDescription
            this.networks = networks

            val networksNames = networks.map { it.name }

            //Set networks spinner for provision a device
            val netAdapter = ArrayAdapter(activity, R.layout.spinner_item, networksNames)
            meshScannerDialogs.setNetworkAdapter(netAdapter)
            meshScannerDialogs.showProvisionDialog(getDefaultName(deviceDescription))
            Log.d(TAG, "deviceInfo " + provisioningModel.selectedDevice!!.device_address)
        }
    }

    private fun getDefaultName(deviceDescription: DeviceDescription): String {
        val address: String = deviceDescription.device_address!!
        return address.substring(address.lastIndex - 4)
    }

    private var provisioningListener = object : ProvisioningStatusPropagator.ProvisioningListener {
        override fun provisioningStatusChanged(provisioningStatus: ProvisioningStatusPropagator.ProvisioningStatus) {
            Log.d(TAG, "provisioningStatusChanged provisioningStatus=$provisioningStatus")

            when (provisioningStatus) {
                ProvisioningStatusPropagator.ProvisioningStatus.DeviceProvisionedGattDisconnected -> {
                    disconnectionCallback()
                }
                ProvisioningStatusPropagator.ProvisioningStatus.ProvisioningSuccessful -> {
                    meshScannerDialogs.updateLoadingText(MeshScannerDialogsView.LoadingText.DeviceSuccessfullyProvisioned)
                    meshScannerDialogs.updateLoadingText(MeshScannerDialogsView.LoadingText.RedirectingToDevices)
                    scannerPresenter.meshLogic.currentSubnet = provisioningModel.networkInfo
                    scannerPresenter.meshLogic.currentNetwork = provisioningModel.networkInfo!!.network

                    scannerPresenter.provisioningSuccessful()

                    val networkFragment = NetworkFragment()
                    var mainActivity = activity as MainActivity
                    mainActivity.showFragment(networkFragment, true, true)
                }
                ProvisioningStatusPropagator.ProvisioningStatus.DeviceAlreadyAdded -> {
                    meshScannerDialogs.showDeviceAlreadyExistDialog(DialogInterface.OnClickListener { dialog, which ->
                        when (which) {
                            AlertDialog.BUTTON_POSITIVE -> {
                                provisioningModel.apply {
                                    val connectableDevice = selectedDevice!!.connectable_device!!

                                    val node = selectedDevice!!.existed_node
                                    if (node != null) {
                                        node.removeOnlyFromLocalStructure()
                                        provisioningModel.apply {
                                            provisionDevice(connectableDevice, getNetworkInfo())
                                        }
                                    } else {
                                        meshScannerDialogs.closeLoadingDialog()
                                    }
                                }
                            }
                            AlertDialog.BUTTON_NEGATIVE -> {
                                meshScannerDialogs.closeLoadingDialog()
                            }
                        }
                    })
                }
            }
        }

        override fun provisioningUnsuccessful(provisioningStatus: ProvisioningStatusPropagator.ProvisioningStatus, error: ErrorType) {
            meshScannerDialogs.updateLoadingText(error, true)
        }
    }

    fun disconnectionCallback() {
        meshScannerDialogs.closeProvisionDialog()
        if (provisioningModel.provisioned) {
            meshScannerDialogs.updateLoadingText(MeshScannerDialogsView.LoadingText.DeviceProvisionedGattDisconnected)
        }
    }

    fun onResume() {
        provisioningModel.provisioningStatusPropagator.addProvisioningStatusListener(provisioningListener)
    }

    fun onPause() {
        provisioningModel.provisioningStatusPropagator.removeProvisioningStatusListener(provisioningListener)
        meshScannerDialogs.closeLoadingDialog()
        meshScannerDialogs.closeProvisionDialog()
    }

    fun validDeviceName(name: String): Boolean {
        return name.trim().isNotEmpty()
    }

    fun provisionClicked(deviceName: String) {
        meshScannerDialogs.showLoadingDialog()
        meshScannerDialogs.updateLoadingText(MeshScannerDialogsView.LoadingText.Provisioning)
        if (!networks.isEmpty()) {
            provisioningModel.provisionDevice(networks[spinner_value], deviceName)
        }
    }

    fun disconnectProvisioningClicked() {
        meshScannerDialogs.closeProvisionDialog()
    }

    fun onNetworkSelected(position: Int) {
        spinner_value = position
    }

}