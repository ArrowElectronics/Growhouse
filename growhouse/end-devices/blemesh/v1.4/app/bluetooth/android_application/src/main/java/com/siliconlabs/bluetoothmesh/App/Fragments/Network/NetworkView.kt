/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.Network

import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlabs.bluetoothmesh.App.ModelView.MeshNode

/**
 * @author Comarch S.A.
 */
interface NetworkView {

    fun showToast(toastMessage: TOAST_MESSAGE)

    fun showErrorToast(errorType: ErrorType)

    fun setMeshIconState(iconState: MESH_ICON_STATE)

    fun showConfigDeviceFragment(deviceInfo: MeshNode)

    fun setActionBarTitle(title: String)

    fun showFragment(fragment: FRAGMENT)

    enum class FRAGMENT {
        GROUP_LIST,
        DEVICE_LIST
    }

    enum class MESH_ICON_STATE {
        DISCONNECTED,
        CONNECTING,
        CONNECTED
    }

    enum class TOAST_MESSAGE {
        NO_NODE_IN_NETWORK,
        NO_PROXY_NODE_IN_NETWORK,

        MESH_NETWORK_DISCONNECTED,
        GATT_NOT_CONNECTED,
        GATT_PROXY_DISCONNECTED,
        GATT_ERROR_DISCOVERING_SERVICES,
        PROXY_SERVICE_NOT_FOUND,
        PROXY_CHARACTERISTIC_NOT_FOUND,
        PROXY_DESCRIPTOR_NOT_FOUND,

        CONNECTION_TIMEOUT
    }
}