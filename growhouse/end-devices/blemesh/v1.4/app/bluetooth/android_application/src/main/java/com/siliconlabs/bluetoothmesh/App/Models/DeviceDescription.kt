/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Models

import com.siliconlab.bluetoothmesh.adk.data_model.node.Node

class DeviceDescription {
    var device_name: String? = null
    var device_address: String? = null
    var rssi: Int = 0
    var tx: Int = 0
    var timestamp: Long = 0
    var position = 0
    var scanRecord: ByteArray? = null
    var connectable_device: BluetoothConnectableDevice? = null
    var existed_node: Node? = null
}
