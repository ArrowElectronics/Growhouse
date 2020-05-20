/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Models

import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlab.bluetoothmesh.adk.data_model.group.Group
import com.siliconlab.bluetoothmesh.adk.functionality_control.ControlGroup
import com.siliconlab.bluetoothmesh.adk.functionality_control.GetGroupStatusCallback
import com.siliconlab.bluetoothmesh.adk.functionality_control.SetGroupStatusCallback
import com.siliconlab.bluetoothmesh.adk.functionality_control.base.ControlRequestParameters
import com.siliconlab.bluetoothmesh.adk.functionality_control.specific.*
import com.siliconlabs.bluetoothmesh.App.Utils.ControlConverters

/**
 * @author Comarch S.A.
 */
class MeshGroupControl(val group: Group) {

    private val controlGroup = ControlGroup(group)
    private val parameters = ControlRequestParameters(1, 1, false)

    fun getOnOff(callback: GetOnOffCallback) {
        controlGroup.getStatus(GenericOnOff(), object : GetGroupStatusCallback<GenericOnOff> {
            override fun success(group: Group?, value: GenericOnOff?) {
                callback.success(value!!.state == GenericOnOff.STATE.ON)
            }

            override fun error(group: Group?, error: ErrorType?) {
                callback.error(error!!)
            }
        })
    }

    fun setOnOff(on: Boolean, callback: SetCallback) {
        val generic = GenericOnOff()
        generic.state = if (on) GenericOnOff.STATE.ON else GenericOnOff.STATE.OFF
        controlGroup.setStatus(generic, parameters, object : SetGroupStatusCallback<GenericOnOff> {
            override fun success(group: Group?, value: GenericOnOff?) {
            }

            override fun error(group: Group?, error: ErrorType?) {
                callback.error(error!!)
            }
        })
    }

    fun getLevel(callback: GetLevelCallback) {
        controlGroup.getStatus(GenericLevel(), object : GetGroupStatusCallback<GenericLevel> {
            override fun success(group: Group?, value: GenericLevel?) {
                callback.success(value!!.level)
            }

            override fun error(group: Group?, error: ErrorType?) {
                callback.error(error!!)
            }
        })
    }

    fun setLevel(levelPercentage: Int, callback: SetCallback) {
        val generic = GenericLevel()
        generic.level = ControlConverters.getLevel(levelPercentage)
        controlGroup.setStatus(generic, parameters, object : SetGroupStatusCallback<GenericLevel> {
            override fun success(group: Group?, value: GenericLevel?) {
            }

            override fun error(group: Group?, error: ErrorType?) {
                callback.error(error!!)
            }
        })
    }

    fun getLightness(callback: GetLightnessCallback) {
        controlGroup.getStatus(LightningLightnessActual(), object : GetGroupStatusCallback<LightningLightnessActual> {
            override fun success(group: Group?, value: LightningLightnessActual?) {
                callback.success(value!!.lightness)
            }

            override fun error(group: Group?, error: ErrorType?) {
                callback.error(error!!)
            }
        })
    }

    fun setLightness(lightnessPercentage: Int, callback: SetCallback) {
        val generic = LightningLightnessActual()
        generic.lightness = ControlConverters.getLightness(lightnessPercentage)
        controlGroup.setStatus(generic, parameters, object : SetGroupStatusCallback<LightningLightnessActual> {
            override fun success(group: Group?, value: LightningLightnessActual?) {
            }

            override fun error(group: Group?, error: ErrorType?) {
                callback.error(error!!)
            }
        })
    }

    fun getColorTemperature(callback: GetColorTemperatureCallback) {
        controlGroup.getStatus(LightningCTLGet(), object : GetGroupStatusCallback<LightningCTLGet> {
            override fun success(group: Group?, value: LightningCTLGet?) {
                callback.success(value!!.lightness, value.temperature)
            }

            override fun error(group: Group?, error: ErrorType?) {
                callback.error(error!!)
            }
        })
    }

    fun setColorTemperature(lightnessPercentage: Int, temperaturePercentage: Int, deltaUvPercentage: Int, callback: SetCallback) {
        val generic = LightningCTLSet()
        generic.lightness = ControlConverters.getLightness(lightnessPercentage)
        generic.temperature = ControlConverters.getTemperature(temperaturePercentage)
        generic.deltaUv = ControlConverters.getDeltaUv(deltaUvPercentage)
        controlGroup.setStatus(generic, parameters, object : SetGroupStatusCallback<LightningCTLSet> {
            override fun success(group: Group?, value: LightningCTLSet?) {
            }

            override fun error(group: Group?, error: ErrorType?) {
                callback.error(error!!)
            }
        })
    }

    fun getColorDeltaUv(callback: GetColorDeltaUvCallback) {
        controlGroup.getStatus(LightningCTLTemperature(), object : GetGroupStatusCallback<LightningCTLTemperature> {
            override fun success(group: Group?, value: LightningCTLTemperature?) {
                callback.success(value!!.temperature, value.deltaUv)
            }

            override fun error(group: Group?, error: ErrorType?) {
                callback.error(error!!)
            }
        })
    }

    interface SetCallback : BaseCallback

    interface GetOnOffCallback : BaseCallback {
        fun success(on: Boolean)
    }

    interface GetLevelCallback : BaseCallback {
        fun success(level: Int)
    }

    interface GetLightnessCallback : BaseCallback {
        fun success(lightness: Int)
    }

    interface GetColorTemperatureCallback : BaseCallback {
        fun success(lightness: Int, temperature: Int)
    }

    interface GetColorDeltaUvCallback : BaseCallback {
        fun success(temperature: Int, deltaUv: Int)
    }

    interface BaseCallback {
        fun error(error: ErrorType)
    }
}