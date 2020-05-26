/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.ModelView

import com.siliconlab.bluetoothmesh.adk.data_model.node.Node
import com.siliconlabs.bluetoothmesh.App.Models.DeviceFunctionality

/**
 * @author Comarch S.A.
 */
data class MeshNode(val node: Node) {
    var onOffState = false
    var levelPercentage = 0
    var lightnessPercentage = 0
    var temperaturePercentage = 0
    var deltaUvPercentage = 0
    var functionality: DeviceFunctionality.FUNCTIONALITY = DeviceFunctionality.FUNCTIONALITY.Unknown
}