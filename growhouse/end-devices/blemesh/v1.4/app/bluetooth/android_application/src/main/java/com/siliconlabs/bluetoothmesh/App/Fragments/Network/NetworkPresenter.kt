/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.Network

import android.util.Log
import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlab.bluetoothmesh.adk.data_model.subnet.Subnet
import com.siliconlabs.bluetoothmesh.App.BasePresenter
import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic
import com.siliconlabs.bluetoothmesh.App.Logic.NetworkConnectionListener
import com.siliconlabs.bluetoothmesh.App.Logic.NetworkConnectionLogic

/**
 * @author Comarch S.A.
 */
class NetworkPresenter(private val networkView: NetworkView, private val networkConnectionLogic: NetworkConnectionLogic, private val meshLogic: MeshLogic) : BasePresenter, NetworkConnectionListener {
    private val TAG: String = javaClass.canonicalName

    private var configDialogShowed: Boolean = false
    private val networkInfo: Subnet = meshLogic.currentSubnet!!

    private var connectToSubnet = true

    override fun onResume() {
        Log.d(TAG, "onResume")
        networkView.setActionBarTitle(networkInfo.name)
        networkConnectionLogic.addListener(this)

        if (connectToSubnet) {
            networkConnectionLogic.connect(networkInfo)
            connectToSubnet = false
        }
    }

    override fun onPause() {
        Log.d(TAG, "onPause")
        networkConnectionLogic.removeListener(this)
    }

    fun onDestroy() {
        networkConnectionLogic.disconnect()
    }

    // View

    fun meshIconClicked(iconState: NetworkView.MESH_ICON_STATE) {
        when (iconState) {
            NetworkView.MESH_ICON_STATE.DISCONNECTED -> {
                networkConnectionLogic.connect(networkInfo)
            }
            NetworkView.MESH_ICON_STATE.CONNECTING -> {
                networkConnectionLogic.disconnect()
            }
            NetworkView.MESH_ICON_STATE.CONNECTED -> {
                networkConnectionLogic.disconnect()
            }
        }
    }

    // NetworkConnectionListener

    override fun connecting() {
        networkView.setMeshIconState(NetworkView.MESH_ICON_STATE.CONNECTING)
    }

    override fun connected() {
        networkView.setMeshIconState(NetworkView.MESH_ICON_STATE.CONNECTED)


        meshLogic.deviceToConfigure?.apply {
            if (!configDialogShowed) {
                networkView.showConfigDeviceFragment(this)
            }
            configDialogShowed = true
        }
    }

    override fun disconnected() {
        networkView.setMeshIconState(NetworkView.MESH_ICON_STATE.DISCONNECTED)
    }

    override fun connectionMessage(message: NetworkConnectionListener.MESSAGE) {
        val message: NetworkView.TOAST_MESSAGE? = when (message) {
            NetworkConnectionListener.MESSAGE.NO_NODE_IN_NETWORK -> NetworkView.TOAST_MESSAGE.NO_NODE_IN_NETWORK
            NetworkConnectionListener.MESSAGE.GATT_NOT_CONNECTED -> NetworkView.TOAST_MESSAGE.GATT_NOT_CONNECTED
            NetworkConnectionListener.MESSAGE.GATT_PROXY_DISCONNECTED -> NetworkView.TOAST_MESSAGE.GATT_PROXY_DISCONNECTED
            NetworkConnectionListener.MESSAGE.GATT_ERROR_DISCOVERING_SERVICES -> NetworkView.TOAST_MESSAGE.GATT_ERROR_DISCOVERING_SERVICES
            NetworkConnectionListener.MESSAGE.PROXY_SERVICE_NOT_FOUND -> NetworkView.TOAST_MESSAGE.PROXY_SERVICE_NOT_FOUND
            NetworkConnectionListener.MESSAGE.PROXY_CHARACTERISTIC_NOT_FOUND -> NetworkView.TOAST_MESSAGE.PROXY_CHARACTERISTIC_NOT_FOUND
            NetworkConnectionListener.MESSAGE.PROXY_DESCRIPTOR_NOT_FOUND -> NetworkView.TOAST_MESSAGE.PROXY_DESCRIPTOR_NOT_FOUND
        }

        message?.let {
            networkView.showToast(message)
        }
    }

    override fun connectionErrorMessage(error: ErrorType) {
        networkView.showErrorToast(error)
    }
}