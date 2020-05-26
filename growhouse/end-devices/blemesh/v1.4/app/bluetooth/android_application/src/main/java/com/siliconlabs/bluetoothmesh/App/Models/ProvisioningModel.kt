/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Models

import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlab.bluetoothmesh.adk.connectable_device.ConnectableDevice
import com.siliconlab.bluetoothmesh.adk.data_model.node.Node
import com.siliconlab.bluetoothmesh.adk.data_model.subnet.Subnet
import com.siliconlab.bluetoothmesh.adk.provisioning.ProvisionerConnection
import com.siliconlab.bluetoothmesh.adk.provisioning.ProvisioningCallback
import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic
import com.siliconlabs.bluetoothmesh.App.Models.ProvisioningStatusPropagator.ProvisioningStatus

class ProvisioningModel(val meshLogic: MeshLogic, val meshNodeManager: MeshNodeManager) : ProvisioningCallback {
    private val TAG: String = javaClass.canonicalName

    val provisioningStatusPropagator = ProvisioningStatusPropagator()

    internal var provisioning = false
    var selectedDevice: DeviceDescription? = null
    internal var provisioned = false
    internal var networkInfo: Subnet? = null
    private var provisionedDeviceName: String = ""

    fun checkIfSelectedDeviceAlreadyAdded(): Boolean {
        val connectableDevice = selectedDevice!!.connectable_device

        meshLogic.currentNetwork?.let {
            it.subnets.forEach {
                it.nodes.forEach {
                    if (it.uuid != null && connectableDevice?.uuid != null) {
                        if (it.uuid!!.contentEquals(connectableDevice.uuid)) {
                            selectedDevice!!.existed_node = it
                            return true
                        }
                    }
                }
            }
        }

        return false
    }

    fun provisionDevice(networkInfo: Subnet, name: String) {
        provisionedDeviceName = name
        this.networkInfo = networkInfo
        meshLogic.currentSubnet = networkInfo
        provisioned = false
        provisioning = true

        val connectableDevice = selectedDevice!!.connectable_device!!
        if (checkIfSelectedDeviceAlreadyAdded()) {
            provisioningStatusPropagator.propagateProvisioningStatus(ProvisioningStatus.DeviceAlreadyAdded)
        } else {
            provisionDevice(connectableDevice, networkInfo)
        }
    }

    fun provisionDevice(connectableDevice: BluetoothConnectableDevice, network: Subnet) {
        val provisionerConnection = ProvisionerConnection(connectableDevice, network)
        provisionerConnection.provisionAsProxy(this)
    }

    override fun success(device: ConnectableDevice, subnet: Subnet, node: Node) {
        provisioningSuccessful(node)
    }

    override fun error(device: ConnectableDevice, subnet: Subnet, error: ErrorType) {
        provisioningUnsuccessful(error)
    }

    fun provisioningSuccessful(node: Node) {
        provisioned = true
        var name = provisionedDeviceName
        if (name.isEmpty()) {
            name = "Unknown"
        }
        node.name = name
        meshLogic.deviceToConfigure = meshNodeManager.getMeshNode(node)
        provisioningStatusPropagator.propagateProvisioningStatus(ProvisioningStatus.ProvisioningSuccessful)
    }


    fun provisioningUnsuccessful(status: ErrorType) {
        provisioningStatusPropagator.propagateProvisioningError(ProvisioningStatus.ErrorDuringProvisioning, status)
    }

    fun getNetworkInfo(): Subnet {
        return networkInfo!!
    }
}