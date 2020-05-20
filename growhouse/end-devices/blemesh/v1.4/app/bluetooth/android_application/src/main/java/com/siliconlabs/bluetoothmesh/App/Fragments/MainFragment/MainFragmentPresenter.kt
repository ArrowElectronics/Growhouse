/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.MainFragment

import com.siliconlabs.bluetoothmesh.App.BasePresenter
import com.siliconlabs.bluetoothmesh.App.Logic.BluetoothStateReceiver

/**
 * @author Comarch S.A.
 */
class MainFragmentPresenter(private val mainFragmentView: MainFragmentView, private val bluetoothStateReceiver: BluetoothStateReceiver) : BasePresenter, BluetoothStateReceiver.BluetoothStateListener {
    private val TAG: String = javaClass.canonicalName

    override fun onResume() {
        mainFragmentView.setView()
        bluetoothStateReceiver.addListener(this)
    }

    override fun onPause() {
        bluetoothStateReceiver.removeListener(this)
    }

    // BluetoothStateListener

    override fun onBluetoothStateChanged(enabled: Boolean) {
        mainFragmentView.setEnablingButtons()
    }
}