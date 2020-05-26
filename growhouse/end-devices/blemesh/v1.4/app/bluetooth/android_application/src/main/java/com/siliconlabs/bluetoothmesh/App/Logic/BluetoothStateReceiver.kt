/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Logic

import android.bluetooth.BluetoothAdapter
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.util.Log

/**
 * @author Comarch S.A.
 */
class BluetoothStateReceiver : BroadcastReceiver() {
    private val TAG: String = javaClass.canonicalName

    private val listeners: ArrayList<BluetoothStateListener> = ArrayList()

    fun addListener(bluetoothStateListener: BluetoothStateListener) {
        synchronized(listeners) {
            listeners.add(bluetoothStateListener)
        }
    }

    fun removeListener(bluetoothStateListener: BluetoothStateListener) {
        synchronized(listeners) {
            listeners.remove(bluetoothStateListener)
        }
    }

    override fun onReceive(context: Context?, intent: Intent?) {
        if (intent?.action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {
            val state = intent?.getIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR)

            when (state) {
                BluetoothAdapter.ERROR,
                BluetoothAdapter.STATE_OFF,
                BluetoothAdapter.STATE_TURNING_OFF -> {
                    notifyListeners(false)
                }
                BluetoothAdapter.STATE_ON -> {
                    notifyListeners(true)
                }
            }
        }
    }

    private fun notifyListeners(enabled: Boolean) {
        synchronized(listeners) {
            listeners.forEach { listener ->
                Log.d(TAG, "onBluetoothStateChanged=$enabled")
                listener.onBluetoothStateChanged(enabled)
            }
        }
    }

    interface BluetoothStateListener {
        fun onBluetoothStateChanged(enabled: Boolean)
    }
}