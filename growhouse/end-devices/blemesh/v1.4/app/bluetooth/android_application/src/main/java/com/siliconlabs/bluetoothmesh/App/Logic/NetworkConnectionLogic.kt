/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Logic

import android.bluetooth.le.ScanCallback
import android.bluetooth.le.ScanResult
import android.content.Context
import android.os.Handler
import android.os.Looper
import android.os.ParcelUuid
import android.util.Log
import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlab.bluetoothmesh.adk.connectable_device.*
import com.siliconlab.bluetoothmesh.adk.connectable_device.ProxyConnection.MESH_PROXY_SERVICE
import com.siliconlab.bluetoothmesh.adk.data_model.node.Node
import com.siliconlab.bluetoothmesh.adk.data_model.subnet.Subnet
import com.siliconlabs.bluetoothmesh.App.Models.BluetoothConnectableDevice
import java.util.*

/**
 * @author Comarch S.A.
 */
class NetworkConnectionLogic(private val context: Context, private val connectableDeviceHelper: ConnectableDeviceHelper, private val bluetoothLeScanLogic: BluetoothScanner) : BluetoothConnectableDevice.DeviceConnectionCallback {
    private val TAG: String = javaClass.canonicalName

    private val uiHandler: Handler = Handler(Looper.getMainLooper())

    enum class CONNECTION_STATE {
        DISCONNECTED,
        CONNECTING,
        CONNECTED
    }

    private var currentState = CONNECTION_STATE.DISCONNECTED

    private val listeners: ArrayList<NetworkConnectionListener> = ArrayList()

    private var networkInfo: Subnet? = null

    var proxyConnection: ProxyConnection? = null

    private var bluetoothConnectableDevice: BluetoothConnectableDevice? = null

    private var connectionTimeoutRunnable: Runnable = Runnable {
        connetionTimeout()
    }

    fun connect(network: Subnet) {
        synchronized(currentState) {
            if (networkInfo != null) {
                // new network
                if (networkInfo != network) {
                    disconnect()
                } else {
                    // already connected/connecting
                    if (currentState != CONNECTION_STATE.DISCONNECTED) {
                        return
                    }
                }
            }

            Log.d(TAG, "Connecting")
            networkInfo = network
            bluetoothLeScanLogic.addScanCallback(scanCallback)
            startScan()
        }
    }

    fun disconnect() {
        Log.d(TAG, "Disconnecting")
        networkInfo = null
        stopScan()
        bluetoothLeScanLogic.removeScanCallback(scanCallback)
        setCurrentState(CONNECTION_STATE.DISCONNECTED)
        bluetoothConnectableDevice?.removeDeviceConnectionCallback(this)
        proxyConnection?.disconnect(object : DisconnectionCallback {
            override fun success(device: ConnectableDevice?) {
                Log.d(TAG, "Disconnecting success")
            }

            override fun error(device: ConnectableDevice?, error: ErrorType?) {
                Log.d(TAG, "Disconnecting error")
            }

        })
    }

    fun addListener(networkConnectionListener: NetworkConnectionListener) {
        synchronized(listeners) {
            listeners.add(networkConnectionListener)

            notifyCurrentState()
        }
    }

    fun removeListener(networkConnectionListener: NetworkConnectionListener) {
        synchronized(listeners) {
            listeners.remove(networkConnectionListener)
        }
    }

    // DeviceConnectionCallback

    override fun onConnectedToDevice() {
        Log.d(TAG, "onConnectedToDevice")
        //ignore
    }

    override fun onDisconnectedFromDevice() {
        Log.d(TAG, "onDisconnectedFromDevice")
        setCurrentState(CONNECTION_STATE.DISCONNECTED)
    }

    // ScanCallback

    private val scanCallback = object : ScanCallback() {
        override fun onScanResult(callbackType: Int, result: ScanResult) {
            Log.d(TAG, result.toString())

            val bluetoothConnectableDevice = BluetoothConnectableDevice(context, result)

            val subnets = connectableDeviceHelper.findSubnets(bluetoothConnectableDevice)

            if (subnets.contains(networkInfo)) {
                stopScan()
            } else {
                return
            }

            // workaround to 133 gatt issue
            // https://github.com/googlesamples/android-BluetoothLeGatt/issues/44
            uiHandler.postDelayed({
                this@NetworkConnectionLogic.bluetoothConnectableDevice = bluetoothConnectableDevice
                bluetoothConnectableDevice.addDeviceConnectionCallback(this@NetworkConnectionLogic)

                proxyConnection = ProxyConnection(bluetoothConnectableDevice)
                proxyConnection!!.connectToProxy(object : ConnectionCallback {
                    override fun success(device: ConnectableDevice) {
                        Log.d(TAG, "ConnectionCallback success")
                        setCurrentState(CONNECTION_STATE.CONNECTED)
                    }

                    override fun error(device: ConnectableDevice, error: ErrorType) {
                        Log.d(TAG, "ConnectionCallback error=$error")
                        setCurrentState(CONNECTION_STATE.DISCONNECTED)
                        connectionErrorMessage(error)
                    }
                })
            }, 500)
        }
    }

    private fun startScan() {
        Log.d(TAG, "Start Scanning")

        networkInfo?.apply {
            if (nodes.isEmpty()) {
                connectionMessage(NetworkConnectionListener.MESSAGE.NO_NODE_IN_NETWORK)
                return
            }

            val meshProxyService = ParcelUuid(MESH_PROXY_SERVICE)
            if (bluetoothLeScanLogic.startLeScan(meshProxyService)) {
                setCurrentState(CONNECTION_STATE.CONNECTING)
                uiHandler.removeCallbacks(connectionTimeoutRunnable)
                uiHandler.postDelayed(connectionTimeoutRunnable, 10000)
            }
        }
    }

    private fun stopScan() {
        Log.d(TAG, "Stop Scanning")

        bluetoothLeScanLogic.stopLeScan()
    }

    private fun connetionTimeout() {
        Log.d(TAG, "Connection timeout")

        stopScan();
        setCurrentState(CONNECTION_STATE.DISCONNECTED)
        connectionErrorMessage(ErrorType(ErrorType.TYPE.COULD_NOT_CONNECT_TO_DEVICE))
    }

    fun isConnected(): Boolean {
        synchronized(currentState) {
            return currentState == CONNECTION_STATE.CONNECTED
        }
    }

    private fun setCurrentState(currentState: CONNECTION_STATE) {
        Log.d(TAG, "setCurrentState: " + currentState)
        synchronized(currentState) {
            uiHandler.removeCallbacks(connectionTimeoutRunnable)
            this.currentState = currentState
        }
        notifyCurrentState()
    }

    private fun notifyCurrentState() {
        synchronized(listeners) {
            uiHandler.post {
                when (currentState) {
                    CONNECTION_STATE.DISCONNECTED -> {
                        listeners.forEach { listener -> listener.disconnected() }
                    }
                    CONNECTION_STATE.CONNECTING -> {
                        listeners.forEach { listener -> listener.connecting() }
                    }
                    CONNECTION_STATE.CONNECTED -> {
                        listeners.forEach { listener -> listener.connected() }
                    }
                }
            }
        }
    }

    private fun connectionMessage(message: NetworkConnectionListener.MESSAGE) {
        synchronized(listeners) {
            uiHandler.post {
                listeners.forEach { listener -> listener.connectionMessage(message) }
            }
        }
    }

    private fun connectionErrorMessage(errorType: ErrorType) {
        synchronized(listeners) {
            uiHandler.post {
                listeners.forEach { listener -> listener.connectionErrorMessage(errorType) }
            }
        }
    }

    fun getCurrentConnectedDeviceInfo(): Node? {
        return connectableDeviceHelper.findNode(bluetoothConnectableDevice)
    }

}