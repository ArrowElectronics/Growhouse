/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Activities.Main

import com.siliconlabs.bluetoothmesh.App.BasePresenter
import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic
import com.siliconlabs.bluetoothmesh.App.Logic.NetworkConnectionLogic

/**
 * @author Comarch S.A.
 */
class MainActivityPresenter(private val mainActivityView: MainActivityView, val meshLogic: MeshLogic, val networkConnectionLogic: NetworkConnectionLogic) : BasePresenter {
    private val TAG: String = javaClass.canonicalName

    private var connectToSubnet = false

    override fun onResume() {
        mainActivityView.setActionBar()

        if (connectToSubnet) {
            meshLogic.currentSubnet?.let {
                networkConnectionLogic.connect(it)
            }
        }
    }

    override fun onPause() {
        connectToSubnet = networkConnectionLogic.isConnected()
        networkConnectionLogic.disconnect()
    }

}