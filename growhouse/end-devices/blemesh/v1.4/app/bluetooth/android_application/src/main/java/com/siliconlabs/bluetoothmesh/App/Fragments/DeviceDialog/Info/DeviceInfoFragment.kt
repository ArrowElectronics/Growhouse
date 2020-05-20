/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.DeviceDialog.Info

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TableRow
import com.siliconlab.bluetoothmesh.adk.data_model.model.Model
import com.siliconlabs.bluetoothmesh.App.ModelView.MeshNode
import com.siliconlabs.bluetoothmesh.App.Utils.Converters
import com.siliconlabs.bluetoothmesh.R
import dagger.android.support.DaggerFragment
import kotlinx.android.synthetic.main.dialog_device_info.*
import kotlinx.android.synthetic.main.table_item.view.*
import javax.inject.Inject

/**
 * @author Comarch S.A.
 */
class DeviceInfoFragment : DaggerFragment(), DeviceInfoView {

    @Inject
    lateinit var deviceInfoPresenter: DeviceInfoPresenter

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        return inflater.inflate(R.layout.dialog_device_info, container, false)
    }

    override fun onResume() {
        super.onResume()
        deviceInfoPresenter.onResume()
    }

    override fun onPause() {
        super.onPause()
        deviceInfoPresenter.onPause()
    }

    override fun setDeviceInfo(deviceInfo: MeshNode) {
        device_name.text = deviceInfo.node.name
        device_address.text = deviceInfo.node.primaryElementAddress?.toString()
        device_uuid.text = Converters.getHexValue(deviceInfo.node.uuid)

        val network = deviceInfo.node.subnets.iterator().next()
        network_name.text = network.name

        key_net.text = Converters.getHexValue(network.netKey.key)
        if (deviceInfo.node.groups.size > 0) {
            key_app.text = Converters.getHexValue(deviceInfo.node.groups.iterator().next().appKey.key)
        }
        key_dev.text = Converters.getHexValue(deviceInfo.node.devKey.key)

        var tableIndex = 0

        deviceInfo.node.elements?.forEachIndexed { elementIndex, element ->
            val models = mutableListOf<Model>()
            models.addAll(element.sigModels)
            models.addAll(element.vendorModels)

            models.forEach { model ->
                val modelName = model.name

                val modelType: String
                val modelId: String

                if (model.isSIGModel) {
                    modelType = "SIG"

                    val sigInfo = Converters.inv_atou16(model.id)
                    modelId = "0x" + Converters.getHexValueNoSpace(sigInfo)
                } else {
                    val vendorInfo = Converters.inv_atou32(model.id)
                    val vendorType = byteArrayOf(vendorInfo[2], vendorInfo[3])
                    val vendorId = byteArrayOf(vendorInfo[0], vendorInfo[1])

                    modelType = "0x" + Converters.getHexValueNoSpace(vendorType)
                    modelId = "0x" + Converters.getHexValueNoSpace(vendorId)
                }

                val modelInfo = DeviceModelInfo(elementIndex, modelType, modelId, modelName)
                elements_table.addView(createRowElement(modelInfo, tableIndex % 2 == 0))
                tableIndex++
            }
        }
    }

    private fun createRowElement(modelInfo: DeviceModelInfo, lightBackground: Boolean): TableRow {
        val row = layoutInflater.inflate(R.layout.table_item, null) as TableRow
        row.apply {
            background = if (lightBackground) {
                context.getDrawable(R.color.dialog_device_config_table_light_background)
            } else {
                context.getDrawable(R.color.dialog_device_config_table_dark_background)
            }

            row_element.text = modelInfo.elementIndex.toString()
            row_vendor.text = modelInfo.modelType
            row_id.text = modelInfo.modelId
            row_description.text = modelInfo.modelName
            row_description.isSelected = true
        }

        return row
    }

    data class DeviceModelInfo(val elementIndex: Int, val modelType: String, val modelId: String, val modelName: String)
}