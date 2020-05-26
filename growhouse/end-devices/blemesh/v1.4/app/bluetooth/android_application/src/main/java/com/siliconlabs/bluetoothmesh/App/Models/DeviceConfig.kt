/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Models

import com.siliconlab.bluetoothmesh.adk.data_model.group.Group

/**
 * @author Comarch S.A.
 */
data class DeviceConfig(var name: String? = null,
                        var proxy: Boolean? = null,
                        var relay: Boolean? = null,
                        var friend: Boolean? = null,
                        var lowPower: Boolean? = null,
                        var functionality: DeviceFunctionality.FUNCTIONALITY? = null,
                        var group: Group? = null)