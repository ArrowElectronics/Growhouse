/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Models

import android.util.Log
import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlab.bluetoothmesh.adk.data_model.group.Group
import com.siliconlab.bluetoothmesh.adk.data_model.group.GroupRemovalCallback
import com.siliconlab.bluetoothmesh.adk.data_model.network.Network
import com.siliconlab.bluetoothmesh.adk.data_model.network.NetworkCreationException
import com.siliconlab.bluetoothmesh.adk.data_model.subnet.GroupCreationException
import com.siliconlab.bluetoothmesh.adk.data_model.subnet.Subnet
import com.siliconlab.bluetoothmesh.adk.data_model.subnet.SubnetCreationException
import com.siliconlab.bluetoothmesh.adk.data_model.subnet.SubnetRemovalCallback
import com.siliconlab.bluetoothmesh.adk.internal.data_model.group.GroupRemovalErrorResult
import com.siliconlab.bluetoothmesh.adk.internal.data_model.subnet.SubnetRemovalErrorResult
import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic

/**
 * @author Comarch S.A.
 */
class MeshNetworkManager(val meshLogic: MeshLogic) {
    private val TAG: String = javaClass.canonicalName

    val bluetoothMesh = meshLogic.bluetoothMesh

    var network: Network? = null
    var subnet: Subnet? = null

    init {
        setupNetwork()
        setupSubnet()
        setupGroup()

        meshLogic.currentNetwork = network
    }

    // setup

    private fun setupNetwork() {
        val networks = bluetoothMesh.networks
        if (networks.isEmpty()) {
            try {
                network = bluetoothMesh.createNetwork("Network")
            } catch (e: NetworkCreationException) {
                Log.e(TAG, e.toString())
            }
        } else {
            network = networks.iterator().next()
        }
    }

    private fun setupSubnet() {
        val subnets = network?.subnets!!
        if (subnets.isEmpty()) {
            try {
                subnet = createSubnet("Demo Network")
            } catch (e: SubnetCreationException) {
                Log.e(TAG, e.toString())
            }
        } else {
            subnet = subnets.iterator().next()
        }
    }

    private fun setupGroup() {
        val groups = subnet?.groups!!
        if (groups.isEmpty()) {
            try {
                subnet?.createGroup("Demo Group")
            } catch (e: GroupCreationException) {
                Log.e(TAG, e.toString())
            }
        }
    }

    // subnet

    fun createSubnet(name: String): Subnet? {
        return network?.createSubnet(name)
    }

    fun removeSubnet(subnet: Subnet, callback: DeleteNetworksCallback) {
        subnet.removeSubnet(object : SubnetRemovalCallback {
            override fun success(subnet: Subnet?) {
                callback.success()
            }

            override fun error(subnet: Subnet?, result: SubnetRemovalErrorResult?, errorType: ErrorType?) {
                callback.error(subnet, errorType)
            }
        })
    }

    // group

    fun createGroup(name: String, subnet: Subnet): Group? {
        return subnet.createGroup(name)
    }

    fun removeGroup(group: Group, callback: DeleteGroupsCallback) {
        group.removeGroup(object : GroupRemovalCallback {
            override fun success(group: Group?) {
                callback.success()
            }

            override fun error(group: Group?, result: GroupRemovalErrorResult?, errorType: ErrorType?) {
                callback.error(group, errorType)
            }
        })
    }

    interface DeleteGroupsCallback {
        fun success()

        fun error(group: Group?, errorType: ErrorType?)
    }

    interface DeleteNetworksCallback {
        fun success()

        fun error(subnet: Subnet?, error: ErrorType?)
    }
}
