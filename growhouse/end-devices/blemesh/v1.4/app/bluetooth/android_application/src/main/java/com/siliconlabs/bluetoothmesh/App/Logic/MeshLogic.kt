/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Logic

import android.content.Context
import android.content.Intent
import android.support.v4.content.FileProvider
import com.siliconlab.bluetoothmesh.adk.BluetoothMesh
import com.siliconlab.bluetoothmesh.adk.configuration.BluetoothMeshConfiguration
import com.siliconlab.bluetoothmesh.adk.data_model.group.Group
import com.siliconlab.bluetoothmesh.adk.data_model.network.Network
import com.siliconlab.bluetoothmesh.adk.data_model.subnet.Subnet
import com.siliconlab.bluetoothmesh.adk.export_data.ExportKeys
import com.siliconlabs.bluetoothmesh.App.ModelView.MeshNode
import java.io.File

/**
 * @author Comarch S.A.
 */
class MeshLogic(val context: Context) {
    private val TAG: String = javaClass.canonicalName

    val bluetoothMesh: BluetoothMesh

    init {
        BluetoothMesh.initialize(context, BluetoothMeshConfiguration(emptyList()))
        bluetoothMesh = BluetoothMesh.getInstance()
    }

    // data
    var currentNetwork: Network? = null
    var currentSubnet: Subnet? = null
    var currentGroup: Group? = null
    var deviceToConfigure: MeshNode? = null

    // export

    fun exportNetworkKeys(networks: Subnet): Intent {
        val networks = hashSetOf(networks.network)
        val exportKeys = ExportKeys(networks)

        val shareIntent = Intent(Intent.ACTION_SEND)
        shareIntent.type = "application/json"

        val fileName = "keys.json"
        val filePath = context.filesDir.path.toString() + "/" + fileName
        var file = File(filePath)
        val export = exportKeys.export();

        file.createNewFile()
        file.writeText(export)

        val uri = FileProvider.getUriForFile(context, "com.siliconlabs.bluetoothmesh.fileprovider", file)

        shareIntent.putExtra(Intent.EXTRA_STREAM, uri);

        return shareIntent
    }
}