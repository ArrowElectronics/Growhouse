/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.DeviceList

import android.util.Log
import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlabs.bluetoothmesh.App.BasePresenter
import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic
import com.siliconlabs.bluetoothmesh.App.Logic.NetworkConnectionListener
import com.siliconlabs.bluetoothmesh.App.Logic.NetworkConnectionLogic
import com.siliconlabs.bluetoothmesh.App.ModelView.MeshNode
import com.siliconlabs.bluetoothmesh.App.Models.DeviceFunctionality
import com.siliconlabs.bluetoothmesh.App.Models.MeshElementControl
import com.siliconlabs.bluetoothmesh.App.Models.MeshNodeManager

/**
 * @author Comarch S.A.
 */
class DeviceListPresenter(private val deviceListView: DeviceListView, val meshLogic: MeshLogic, val networkConnectionLogic: NetworkConnectionLogic, val meshNodeManager: MeshNodeManager) : BasePresenter, DeviceListAdapter.DeviceItemListener, MeshElementControl.SetCallback, NetworkConnectionListener {
    private val TAG: String = javaClass.canonicalName

    private val networkInfo = meshLogic.currentSubnet!!

    private var deviceList: Set<MeshNode> = emptySet()

    private var startedConfiguration = false

    override fun onResume() {
        Log.d(TAG, "onResume")
        refreshList()
        networkConnectionLogic.addListener(this)
        if (meshLogic.deviceToConfigure != null && !startedConfiguration) {
            startedConfiguration = true
            deviceListView.showLoadingDialog()
            deviceListView.updateLoadingDialogMessage(DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_CONNECTING)
        }
    }

    override fun onPause() {
        Log.d(TAG, "onPause")
    }

    fun refreshList() {
        deviceList = meshNodeManager.getMeshNodes(networkInfo)
        deviceListView.setDevicesList(deviceList)
    }

    override fun onClickDeviceImageListener(deviceInfo: MeshNode) {
        if (deviceInfo.node.groups.isEmpty()) {
            return
        }

        val nodeElementControl = MeshElementControl(deviceInfo.node.elements[0], deviceInfo.node.groups.iterator().next())

        when (deviceInfo.functionality) {
            DeviceFunctionality.FUNCTIONALITY.OnOff -> {
                val newOnOffState = !deviceInfo.onOffState

                nodeElementControl.setOnOff(newOnOffState, this)
                deviceInfo.onOffState = newOnOffState
            }
            DeviceFunctionality.FUNCTIONALITY.Level -> {
                var newLevelPercentage = 100
                if (deviceInfo.levelPercentage > 0) {
                    newLevelPercentage = 0
                }

                nodeElementControl.setLevel(newLevelPercentage, this)
                deviceInfo.levelPercentage = newLevelPercentage
            }
            DeviceFunctionality.FUNCTIONALITY.Lightness -> {
                var newLightnessPercentage = 100
                if (deviceInfo.lightnessPercentage > 0) {
                    newLightnessPercentage = 0
                }

                nodeElementControl.setLightness(newLightnessPercentage, this)
                deviceInfo.lightnessPercentage = newLightnessPercentage
            }
            DeviceFunctionality.FUNCTIONALITY.CTL -> {
                var newLightnessPercentage = 100
                if (deviceInfo.lightnessPercentage > 0) {
                    newLightnessPercentage = 0
                }

                nodeElementControl.setColorTemperature(newLightnessPercentage, deviceInfo.temperaturePercentage, deviceInfo.deltaUvPercentage, this)
                deviceInfo.lightnessPercentage = newLightnessPercentage
            }
        }

        deviceListView.notifyDataSetChanged()
    }

    override fun onDeleteClickListener(deviceInfo: List<MeshNode>) {
        deviceListView.showDeleteDeviceDialog(deviceInfo)
    }

    override fun onSeekBarChangeListener(deviceInfo: MeshNode, levelPercentage: Int, temperaturePercentage: Int?, deltaUvPercentage: Int?) {
        val nodeElementControl = MeshElementControl(deviceInfo.node.elements[0], deviceInfo.node.groups.iterator().next())

        when (deviceInfo.functionality) {
            DeviceFunctionality.FUNCTIONALITY.Level -> {
                nodeElementControl.setLevel(levelPercentage, this)

                deviceInfo.levelPercentage = levelPercentage
            }
            DeviceFunctionality.FUNCTIONALITY.Lightness -> {
                nodeElementControl.setLightness(levelPercentage, this)

                deviceInfo.lightnessPercentage = levelPercentage
            }
            DeviceFunctionality.FUNCTIONALITY.CTL -> {
                if (temperaturePercentage != null && deltaUvPercentage != null) {
                    nodeElementControl.setColorTemperature(levelPercentage, temperaturePercentage, deltaUvPercentage, this)

                    deviceInfo.lightnessPercentage = levelPercentage
                    deviceInfo.temperaturePercentage = temperaturePercentage
                    deviceInfo.deltaUvPercentage = deltaUvPercentage
                }
            }
        }

        deviceListView.notifyDataSetChanged()
    }

    override fun onConfigClickListener(deviceInfo: MeshNode) {
        deviceListView.showDeviceConfigDialog(deviceInfo)
    }


    fun startConfigDevice(deviceInfo: MeshNode) {
        deviceListView.startConfigDevice(deviceInfo)
    }

    // mesh element control callback

    override fun error(error: ErrorType) {
        deviceListView.showErrorToast(error)
    }

    // network connection callback

    override fun connecting() {
        deviceListView.notifyDataSetChanged()
    }

    override fun connected() {
        deviceListView.notifyDataSetChanged()
    }

    override fun disconnected() {
        deviceListView.notifyDataSetChanged()
    }

    override fun connectionMessage(message: NetworkConnectionListener.MESSAGE) {
        // nothing to do
    }

    override fun connectionErrorMessage(error: ErrorType) {
        // nothing to do
        if (startedConfiguration) {
            deviceListView.updateLoadingDialogMessage(DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_CONNECTING_ERROR, error, true)
        }
    }
}
