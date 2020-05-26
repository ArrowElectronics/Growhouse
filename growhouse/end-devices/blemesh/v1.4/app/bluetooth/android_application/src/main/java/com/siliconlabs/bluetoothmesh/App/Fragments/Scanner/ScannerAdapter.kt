/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.Scanner

import android.content.Context
import android.support.v7.widget.RecyclerView.Adapter
import android.support.v7.widget.RecyclerView.ViewHolder
import android.view.LayoutInflater
import android.view.ViewGroup
import android.widget.ArrayAdapter
import com.siliconlabs.bluetoothmesh.App.Models.DeviceDescription
import com.siliconlabs.bluetoothmesh.R
import kotlinx.android.synthetic.main.scanner_adapter.view.*

class ScannerAdapter(val context: Context, val list: ArrayList<DeviceDescription> = ArrayList(), val presenter: ScannerPresenter) : Adapter<ViewHolder>() {

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        return object : ViewHolder(LayoutInflater.from(parent.context).inflate(R.layout.scanner_adapter, parent, false)) {}
    }

    override fun getItemCount(): Int {
        return list.size
    }

    override fun onBindViewHolder(holder: ViewHolder, position: Int) {
        fillViewHolder(holder, position)
    }

    private fun fillViewHolder(viewHolder: ViewHolder, position: Int) {
        val deviceDescription = list[position]

        viewHolder.itemView.apply {
            if (deviceDescription.device_name.isNullOrEmpty()) {
                tv_device_name.text = context.getString(R.string.scanner_adapter_device_unknown)
            } else {
                tv_device_name.text = deviceDescription.device_name
            }
            tv_device_rssi.text = context.getString(R.string.scanner_adapter_rssi).format(deviceDescription.rssi)
            tv_device_address.text = deviceDescription.device_address

            btn_provision_device.setOnClickListener {
                presenter.onItemClicked(list[position])
            }
        }
    }

    fun clearAdapter() {
        list.clear()
        notifyDataSetChanged()
    }

    fun addDevice(deviceDescription: DeviceDescription) {
        val pastDeviceIndex = getDeviceIndexByAddress(deviceDescription.device_address)
        if (pastDeviceIndex >= 0) {
            val pastDevice = list[pastDeviceIndex]
            list[pastDeviceIndex] = deviceDescription
            if (pastDevice != deviceDescription) {
                notifyDataSetChanged()
            }
        } else {
            list.add(deviceDescription)
            notifyDataSetChanged()
        }
    }

    private fun getDeviceIndexByAddress(deviceAddress: String?): Int {
        return list.indexOfFirst { deviceInfo -> deviceInfo.device_address.equals(deviceAddress) }
    }
}