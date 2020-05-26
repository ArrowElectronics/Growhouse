/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.NetworkList

import android.content.Intent
import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlab.bluetoothmesh.adk.data_model.subnet.Subnet

/**
 * @author Comarch S.A.
 */
interface NetworkListView {
    fun showDeleteNetworkDialog(networkInfo: List<Subnet>)

    fun showDeleteNetworkLocallyDialog(subnet: Subnet, errorType: ErrorType)

    fun showEditNetworkDialog(networkInfo: Subnet)

    fun showAddNetworkDialog()

    fun showToast(toastMessage: NetworkListView.TOAST_MESSAGE)

    fun setNetworkList(networkList: Set<Subnet>)

    fun showIntent(intent: Intent)

    fun showLoadingDialog()

    fun updateLoadingDialogMessage(loadingMessage: LOADING_DIALOG_MESSAGE, message: String = "", showCloseButton: Boolean = false)

    fun updateLoadingDialogMessage(loadingMessage: LOADING_DIALOG_MESSAGE, errorType: ErrorType, showCloseButton: Boolean = false)

    fun dismissLoadingDialog()

    fun showNetworkFragment()

    enum class LOADING_DIALOG_MESSAGE {
        CONNECTING_TO_NETWORK,
        CONNECTING_TO_NETWORK_ERROR,
        REMOVING_NETWORK,
        REMOVING_NETWORK_ERROR
    }

    enum class TOAST_MESSAGE {
        SUCCESS_DELETE,
        SUCCESS_UPDATE,
        SUCCESS_ADD,
        ERROR_CREATING_NETWORK
    }

}