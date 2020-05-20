/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.DeviceList

import android.content.Context
import android.support.v7.widget.RecyclerView
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.SeekBar
import com.siliconlabs.bluetoothmesh.App.Logic.NetworkConnectionLogic
import com.siliconlabs.bluetoothmesh.App.ModelView.MeshNode
import com.siliconlabs.bluetoothmesh.App.Models.DeviceFunctionality
import com.siliconlabs.bluetoothmesh.App.Utils.ControlConverters
import com.siliconlabs.bluetoothmesh.App.Views.MultiChoiceArrayAdapter
import com.siliconlabs.bluetoothmesh.R
import kotlinx.android.synthetic.main.devices_adapter.view.*
import java.util.*

/**
 * @author Comarch S.A.
 */
class DeviceListAdapter(ctx: Context, private val networkConnectionLogic: NetworkConnectionLogic, private val deviceItemListener: DeviceItemListener) :
        MultiChoiceArrayAdapter<MeshNode>(ctx, 0) {

    override fun onDeleteClickListener(item: List<MeshNode>) {
        deviceItemListener.onDeleteClickListener(item)
    }

    fun onItemClickListener(view: View) {

    }

    override fun getView(position: Int, convertView: View?, parent: ViewGroup?): View {
        val deviceInfo = getItem(position)
        var holder: ViewHolder
        var view: View

        if (convertView == null) {
            view = LayoutInflater.from(context).inflate(R.layout.devices_adapter, parent, false)
            holder = ViewHolder(view)
            view.tag = holder
        } else {
            view = convertView
            holder = convertView.tag as ViewHolder
        }

        holder.itemView.apply {
            tv_device_name.text = deviceInfo.node.name
            tv_device_id.text = deviceInfo.node.primaryElementAddress?.toString()

            cb_selected.isChecked = selectedItemsIds.indexOfKey(position) >= 0

            iv_config.setOnClickListener {
                deviceItemListener.onConfigClickListener(deviceInfo)
            }

            when (deviceInfo.functionality) {
                DeviceFunctionality.FUNCTIONALITY.OnOff -> {
                    iv_device_image.setOnClickListener(ClickDeviceImageListener(deviceInfo))
                    ll_controlls.visibility = View.GONE

                    if (!networkConnectionLogic.isConnected()) {
                        iv_device_image.setImageResource(R.drawable.toggle_off)
                    } else if (deviceInfo.onOffState) {
                        iv_device_image.setImageResource(R.drawable.toggle_on)
                    } else {
                        iv_device_image.setImageResource(R.drawable.toggle_off)
                    }
                }
                DeviceFunctionality.FUNCTIONALITY.OnOffClient -> {
                    ll_controlls.visibility = View.GONE

                    if (!networkConnectionLogic.isConnected()) {
                        iv_device_image.setImageResource(R.drawable.toggle_off)
                    } else {
                        iv_device_image.setImageResource(R.drawable.toggle_on)
                    }
                }
                DeviceFunctionality.FUNCTIONALITY.Lightness -> {
                    iv_device_image.setOnClickListener(ClickDeviceImageListener(deviceInfo))
                    ll_controlls.visibility = View.VISIBLE
                    ll_slider.visibility = View.VISIBLE
                    ll_slider_CTL.visibility = View.GONE
                    sb_level_control.progress = deviceInfo.lightnessPercentage
                    sb_level_control.setOnSeekBarChangeListener(ControlChangeListener(deviceInfo))
                    tv_level_value.text = context.getString(R.string.device_adapter_lightness_value).format(sb_level_control.progress)

                    if (!networkConnectionLogic.isConnected()) {
                        iv_device_image.setImageResource(R.drawable.toggle_off)
                        setEnabledControls(this, false)
                    } else if (deviceInfo.lightnessPercentage > 0) {
                        iv_device_image.setImageResource(R.drawable.toggle_on)
                        setEnabledControls(this, true)
                    } else {
                        iv_device_image.setImageResource(R.drawable.toggle_off)
                        setEnabledControls(this, true)
                    }
                }
                DeviceFunctionality.FUNCTIONALITY.LightnessClient -> {
                    ll_controlls.visibility = View.GONE

                    if (!networkConnectionLogic.isConnected()) {
                        iv_device_image.setImageResource(R.drawable.toggle_off)
                    } else {
                        iv_device_image.setImageResource(R.drawable.toggle_on)
                    }
                }
                DeviceFunctionality.FUNCTIONALITY.Level -> {
                    iv_device_image.setOnClickListener(ClickDeviceImageListener(deviceInfo))
                    ll_controlls.visibility = View.VISIBLE
                    ll_slider.visibility = View.VISIBLE
                    ll_slider_CTL.visibility = View.GONE
                    sb_level_control.progress = deviceInfo.levelPercentage
                    sb_level_control.setOnSeekBarChangeListener(ControlChangeListener(deviceInfo))
                    tv_level_value.text = context.getString(R.string.device_adapter_lightness_value).format(sb_level_control.progress)

                    if (!networkConnectionLogic.isConnected()) {
                        iv_device_image.setImageResource(R.drawable.lamp_disabled)
                        setEnabledControls(this, false)
                    } else if (deviceInfo.levelPercentage > 0) {
                        iv_device_image.setImageResource(R.drawable.lamp_on)
                        setEnabledControls(this, true)
                    } else {
                        iv_device_image.setImageResource(R.drawable.lamp_off)
                        setEnabledControls(this, true)
                    }
                }
                DeviceFunctionality.FUNCTIONALITY.CTL -> {
                    iv_device_image.setOnClickListener(ClickDeviceImageListener(deviceInfo))
                    ll_controlls.visibility = View.VISIBLE
                    ll_slider.visibility = View.VISIBLE
                    ll_slider_CTL.visibility = View.VISIBLE

                    sb_level_control.progress = deviceInfo.lightnessPercentage
                    tv_level_value.text = context.getString(R.string.device_adapter_lightness_value).format(sb_level_control.progress)

                    sb_temperature_control.progress = deviceInfo.temperaturePercentage
                    tv_temperature_value.text = context.getString(R.string.device_adapter_temperature_value).format(ControlConverters.getTemperature(deviceInfo.temperaturePercentage))

                    sb_uv_control.progress = deviceInfo.deltaUvPercentage
                    tv_uv_value.text = context.getString(R.string.device_adapter_delta_uv_value).format(ControlConverters.getDeltaUvToShow(deviceInfo.deltaUvPercentage))

                    sb_level_control.setOnSeekBarChangeListener(CTLControlChangeListener(deviceInfo, sb_level_control, sb_temperature_control, sb_uv_control))
                    sb_temperature_control.setOnSeekBarChangeListener(CTLControlChangeListener(deviceInfo, sb_level_control, sb_temperature_control, sb_uv_control))
                    sb_uv_control.setOnSeekBarChangeListener(CTLControlChangeListener(deviceInfo, sb_level_control, sb_temperature_control, sb_uv_control))

                    if (!networkConnectionLogic.isConnected()) {
                        iv_device_image.setImageResource(R.drawable.lamp_disabled)
                        setEnabledControls(this, false)
                    } else if (deviceInfo.lightnessPercentage > 0) {
                        iv_device_image.setImageResource(R.drawable.lamp_on)
                        setEnabledControls(this, true)
                    } else {
                        iv_device_image.setImageResource(R.drawable.lamp_off)
                        setEnabledControls(this, true)
                    }
                }
                DeviceFunctionality.FUNCTIONALITY.CTLClient -> {
                    ll_controlls.visibility = View.GONE

                    if (!networkConnectionLogic.isConnected()) {
                        iv_device_image.setImageResource(R.drawable.lamp_off)
                    } else {
                        iv_device_image.setImageResource(R.drawable.lamp_on)
                    }
                }
                else -> {
                    iv_device_image.setImageResource(R.drawable.ic_unknown)
                    iv_device_image.setOnClickListener(ClickDeviceImageListener(deviceInfo))
                    ll_controlls.visibility = View.GONE
                }
            }

            if (multipleMode) {
                cb_selected.visibility = View.VISIBLE
                iv_config.visibility = View.VISIBLE
            } else {
                cb_selected.visibility = View.GONE
                iv_config.visibility = View.GONE
            }

            setEnabledControls(this, networkConnectionLogic.isConnected() && !multipleMode)
        }

        return view
    }

    override fun addAll(collection: MutableCollection<out MeshNode>?) {
        super.addAll(collection)
        sort(DeviceInfoComparator())
    }

    private fun setEnabledControls(view: View, enabled: Boolean) {
        setEnabledView(view.iv_device_image, enabled)
        setEnabledView(view.sb_level_control, enabled)
        setEnabledView(view.tv_level_value, enabled)
        setEnabledView(view.sb_temperature_control, enabled)
        setEnabledView(view.tv_temperature_value, enabled)
        setEnabledView(view.sb_uv_control, enabled)
        setEnabledView(view.tv_uv_value, enabled)
    }

    private fun setEnabledView(view: View, enabled: Boolean) {
        view.isEnabled = enabled
        view.alpha = if (enabled) 1f else 0.5f
    }

    inner class CTLControlChangeListener(private val deviceInfo: MeshNode, private val levelSeekBar: SeekBar, private val temperatureSeekBar: SeekBar, private val uvSeekBar: SeekBar) : SeekBar.OnSeekBarChangeListener {
        override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
        }

        override fun onStartTrackingTouch(seekBar: SeekBar?) {
        }

        override fun onStopTrackingTouch(seekBar: SeekBar?) {
            seekBar?.let {
                this@DeviceListAdapter.deviceItemListener.onSeekBarChangeListener(deviceInfo, levelSeekBar.progress, temperatureSeekBar.progress, uvSeekBar.progress)
            }
        }
    }

    inner class ControlChangeListener(private val deviceInfo: MeshNode) : SeekBar.OnSeekBarChangeListener {
        override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
        }

        override fun onStartTrackingTouch(seekBar: SeekBar?) {
        }

        override fun onStopTrackingTouch(seekBar: SeekBar?) {
            seekBar?.apply {
                this@DeviceListAdapter.deviceItemListener.onSeekBarChangeListener(deviceInfo, progress)
            }
        }
    }

    inner class ClickDeviceImageListener(private val deviceInfo: MeshNode) : View.OnClickListener {
        override fun onClick(v: View?) {
            this@DeviceListAdapter.deviceItemListener.onClickDeviceImageListener(deviceInfo)
        }
    }

    interface DeviceItemListener {
        fun onClickDeviceImageListener(deviceInfo: MeshNode)

        fun onSeekBarChangeListener(deviceInfo: MeshNode, levelPercentage: Int, temperaturePercentage: Int? = null, deltaUvPercentage: Int? = null)

        fun onDeleteClickListener(deviceInfo: List<MeshNode>)

        fun onConfigClickListener(deviceInfo: MeshNode)
    }

    private class ViewHolder(view: View) : RecyclerView.ViewHolder(view)

    // Comparator

    class DeviceInfoComparator : Comparator<MeshNode> {
        override fun compare(o1: MeshNode, o2: MeshNode): Int {
            return o1.node.devKey.keyIndex.compareTo(o2.node.devKey.keyIndex)
        }
    }
}