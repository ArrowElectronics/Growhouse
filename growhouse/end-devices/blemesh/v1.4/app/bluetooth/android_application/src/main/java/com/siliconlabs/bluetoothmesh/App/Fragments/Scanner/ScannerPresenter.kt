/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.Scanner

import android.bluetooth.le.ScanCallback
import android.bluetooth.le.ScanResult
import android.content.Context
import android.os.ParcelUuid
import com.siliconlab.bluetoothmesh.adk.BluetoothMesh
import com.siliconlab.bluetoothmesh.adk.provisioning.ProvisionerConnection.MESH_UNPROVISIONED_SERVICE
import com.siliconlabs.bluetoothmesh.App.BasePresenter
import com.siliconlabs.bluetoothmesh.App.Logic.BluetoothScanner
import com.siliconlabs.bluetoothmesh.App.Logic.BluetoothStateReceiver
import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic
import com.siliconlabs.bluetoothmesh.App.Models.BluetoothConnectableDevice
import com.siliconlabs.bluetoothmesh.App.Models.DeviceDescription
import com.siliconlabs.bluetoothmesh.App.Models.DeviceDescriptionBuilder
import com.siliconlabs.bluetoothmesh.App.Models.ProvisioningModel


class ScannerPresenter(val scannerView: ScannerView, val context: Context, val meshLogic: MeshLogic, provisioningModel: ProvisioningModel, val bluetoothLeScanLogic: BluetoothScanner, val bluetoothStateReceiver: BluetoothStateReceiver) : BasePresenter, BluetoothStateReceiver.BluetoothStateListener {

    var fragmentAttached = false
    private lateinit var bluetoothMesh: BluetoothMesh
    val scannerDialogsPresenter: ScannerDialogsPresenter = ScannerDialogsPresenter(this, provisioningModel)

    override fun onResume() {
        this.fragmentAttached = true
        bluetoothMesh = meshLogic.bluetoothMesh

        scannerDialogsPresenter.init(scannerView.getActivity()!!)
        scannerDialogsPresenter.onResume()

        bluetoothLeScanLogic.addScanCallback(scanCallback)
        bluetoothStateReceiver.addListener(this)
    }

    override fun onPause() {
        bluetoothLeScanLogic.stopLeScan()
        scannerDialogsPresenter.onPause()
        this.fragmentAttached = false
        bluetoothLeScanLogic.removeScanCallback(scanCallback)
        bluetoothStateReceiver.removeListener(this)
    }

    private val scanCallback = object : ScanCallback() {
        override fun onScanResult(callbackType: Int, result: ScanResult?) {
            super.onScanResult(callbackType, result)
            if (result?.scanRecord == null ||
                    result.scanRecord!!.serviceUuids == null ||
                    result.scanRecord!!.serviceUuids.isEmpty()) {
                return
            }

            val bluetoothConnectableDevice = BluetoothConnectableDevice(context, result)
            val unprovisionedDevices = meshLogic.bluetoothMesh.connectableDeviceHelper.filterUnprovisionedDevices(listOf(bluetoothConnectableDevice))

            if (unprovisionedDevices.isNotEmpty()) {
                val deviceDescription = DeviceDescriptionBuilder.build(result, bluetoothConnectableDevice)
                addDevice(deviceDescription)
            }
        }
    }

    private fun addDevice(deviceDescription: DeviceDescription) {
        if (fragmentAttached) {
            scannerView.addDevice(deviceDescription)
        }
    }

    fun startStopScanClicked() {
        if (bluetoothLeScanLogic.isLeScanStarted()) {
            bluetoothLeScanLogic.stopLeScan()
        } else {
            val meshServ = ParcelUuid.fromString(MESH_UNPROVISIONED_SERVICE.toString())
            bluetoothLeScanLogic.startLeScan(meshServ)
            scannerView.clearAdapter()
        }
        scannerView.setScanMenuToActive(bluetoothLeScanLogic.isLeScanStarted())
    }

    fun onItemClicked(deviceDescription: DeviceDescription) {
        val networks = meshLogic.currentNetwork!!.subnets!!.sortedBy { it.name }
        if (networks.isNotEmpty()) {
            scannerDialogsPresenter.configDevice(deviceDescription, networks)
        } else {
            scannerView.showToast(ScannerView.TOAST_MESSAGE.NO_NETWORK_AVAILABLE)
        }
    }

    fun provisioningSuccessful() {
        scannerView.clearAdapter()
    }

    // BluetoothStateListener

    override fun onBluetoothStateChanged(enabled: Boolean) {
        if (!enabled) {
            scannerView.setScanMenuToActive(false)
            scannerView.clearAdapter()
        }
    }
}
