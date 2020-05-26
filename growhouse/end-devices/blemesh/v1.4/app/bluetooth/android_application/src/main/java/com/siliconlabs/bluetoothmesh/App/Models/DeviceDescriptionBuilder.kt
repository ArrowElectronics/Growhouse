/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Models

import android.bluetooth.le.ScanResult

class DeviceDescriptionBuilder {
    companion object {
        fun build(result: ScanResult, device: BluetoothConnectableDevice): DeviceDescription {
            val deviceDescription = DeviceDescription()
            deviceDescription.device_address = result.device.address
            deviceDescription.device_name = result.device.name
            deviceDescription.rssi = result.rssi
            deviceDescription.connectable_device = device
            deviceDescription.tx = result.scanRecord!!.txPowerLevel
            deviceDescription.timestamp = result.timestampNanos
            deviceDescription.scanRecord = result.scanRecord!!.bytes

            return deviceDescription
        }
    }
}