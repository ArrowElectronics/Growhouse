/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Logic

import android.bluetooth.BluetoothAdapter
import android.bluetooth.le.*
import android.os.ParcelUuid
import android.util.Log
import java.util.*

/**
 * @author Comarch S.A.
 */
class BluetoothScanner(bluetoothStateReceiver: BluetoothStateReceiver) : ScanCallback(), BluetoothStateReceiver.BluetoothStateListener {
    private val TAG: String = javaClass.canonicalName

    private var leScanStarted: Boolean = false

    private var bluetoothLeScanner: BluetoothLeScanner? = null

    private val scanCallbacks = ArrayList<ScanCallback>()

    init {
        bluetoothStateReceiver.addListener(this)
    }

    fun addScanCallback(scanCallback: ScanCallback) {
        synchronized(scanCallbacks) {
            scanCallbacks.add(scanCallback)
        }
    }

    fun removeScanCallback(scanCallback: ScanCallback) {
        synchronized(scanCallbacks) {
            scanCallbacks.remove(scanCallback)
        }
    }

    fun startLeScan(meshServ: ParcelUuid): Boolean {
        synchronized(leScanStarted) {
            Log.d(TAG, "startLeScan")
            if (leScanStarted) {
                return true
            }
            if (!BluetoothAdapter.getDefaultAdapter().isEnabled) {
                return false
            }

            initBluetoothLeScanner()

            val settings = ScanSettings.Builder()
                    .setScanMode(ScanSettings.SCAN_MODE_BALANCED)
                    .build()
            val filters = ArrayList<ScanFilter>()
            val filter = ScanFilter.Builder()
                    .setServiceUuid(meshServ)
                    .build()
            filters.add(filter)
            bluetoothLeScanner?.apply {
                startScan(filters, settings, this@BluetoothScanner)
                leScanStarted = true
                return true
            }
            return false
        }
    }

    fun stopLeScan() {
        synchronized(leScanStarted) {
            Log.d(TAG, "stopLeScan")
            if (!leScanStarted) {
                return
            }
            bluetoothLeScanner?.apply {
                stopScan(this@BluetoothScanner)
                leScanStarted = false
            }
        }
    }

    fun isLeScanStarted(): Boolean {
        return leScanStarted
    }

    //

    private fun initBluetoothLeScanner() {
        val bluetoothAdapter = BluetoothAdapter.getDefaultAdapter()
        bluetoothLeScanner = bluetoothAdapter.bluetoothLeScanner
    }

    // ScanCallback

    override fun onScanResult(callbackType: Int, result: ScanResult) {
        super.onScanResult(callbackType, result)
        synchronized(leScanStarted) {
            if (!leScanStarted) {
                return
            }
        }
        synchronized(scanCallbacks) {
            scanCallbacks.forEach { callback ->
                callback.onScanResult(callbackType, result)
            }
        }
    }

    // BluetoothStateListener

    override fun onBluetoothStateChanged(enabled: Boolean) {
        synchronized(leScanStarted) {
            Log.d(TAG, "onBluetoothStateChanged")
            if (leScanStarted) {
                leScanStarted = false
            }
        }
    }
}