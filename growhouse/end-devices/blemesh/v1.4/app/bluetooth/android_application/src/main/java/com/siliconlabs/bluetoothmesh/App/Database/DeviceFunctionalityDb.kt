/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Database

import android.content.Context
import android.content.SharedPreferences
import com.siliconlab.bluetoothmesh.adk.data_model.node.Node
import com.siliconlabs.bluetoothmesh.App.Models.DeviceFunctionality

/**
 * @author Comarch S.A.
 */
class DeviceFunctionalityDb(context: Context) {
    private val FILE_NAME = "nodeFunctionality"

    private val sharedPreferences: SharedPreferences

    init {
        sharedPreferences = context.getSharedPreferences(FILE_NAME, Context.MODE_PRIVATE)
    }

    fun getFunctionality(node: Node): DeviceFunctionality.FUNCTIONALITY {
        val funcOrdinal = sharedPreferences.getInt(node.uuid!!.contentToString(), DeviceFunctionality.FUNCTIONALITY.Unknown.ordinal)
        return DeviceFunctionality.FUNCTIONALITY.values()[funcOrdinal]
    }

    fun saveFunctionality(node: Node, func: DeviceFunctionality.FUNCTIONALITY) {
        val editor = sharedPreferences.edit()
        editor.putInt(node.uuid!!.contentToString(), func.ordinal)
        editor.apply()
    }

    fun removeFunctionality(node: Node) {
        val editor = sharedPreferences.edit()
        editor.remove(node.uuid!!.contentToString())
        editor.apply()
    }
}