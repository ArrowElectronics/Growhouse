/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Models

import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlab.bluetoothmesh.adk.data_model.element.Element
import com.siliconlab.bluetoothmesh.adk.data_model.group.Group
import com.siliconlab.bluetoothmesh.adk.functionality_control.ControlElement
import com.siliconlab.bluetoothmesh.adk.functionality_control.GetElementStatusCallback
import com.siliconlab.bluetoothmesh.adk.functionality_control.SetElementStatusCallback
import com.siliconlab.bluetoothmesh.adk.functionality_control.base.ControlRequestParameters
import com.siliconlab.bluetoothmesh.adk.functionality_control.specific.*
import com.siliconlabs.bluetoothmesh.App.Utils.ControlConverters

/**
 * @author Comarch S.A.
 */
class MeshElementControl(element: Element, val group: Group) {

    private val controlElement = ControlElement(element, group)
    private val parameters = ControlRequestParameters(1, 1, false)

    fun getOnOff(callback: GetOnOffCallback) {
        controlElement.getStatus(GenericOnOff(), object : GetElementStatusCallback<GenericOnOff> {
            override fun success(element: Element?, group: Group?, value: GenericOnOff?) {
                callback.success(value!!.state == GenericOnOff.STATE.ON)
            }

            override fun error(element: Element?, group: Group?, error: ErrorType?) {
                callback.error(error!!)
            }
        })
    }

    fun setOnOff(on: Boolean, callback: SetCallback) {
        val generic = GenericOnOff()
        generic.state = if (on) GenericOnOff.STATE.ON else GenericOnOff.STATE.OFF
        controlElement.setStatus(generic, parameters, object : SetElementStatusCallback<GenericOnOff> {
            override fun success(element: Element?, group: Group?, value: GenericOnOff?) {
            }

            override fun error(element: Element?, group: Group?, error: ErrorType?) {
                callback.error(error!!)
            }
        })
    }

    fun getLevel(callback: GetLevelCallback) {
        controlElement.getStatus(GenericLevel(), object : GetElementStatusCallback<GenericLevel> {
            override fun success(element: Element?, group: Group?, value: GenericLevel?) {
                callback.success(value!!.level)
            }

            override fun error(element: Element?, group: Group?, error: ErrorType?) {
                callback.error(error!!)
            }
        })
    }

    fun setLevel(levelPercentage: Int, callback: SetCallback) {
        val generic = GenericLevel()
        generic.level = ControlConverters.getLevel(levelPercentage)
        controlElement.setStatus(generic, parameters, object : SetElementStatusCallback<GenericLevel> {
            override fun success(element: Element?, group: Group?, value: GenericLevel?) {
            }

            override fun error(element: Element?, group: Group?, error: ErrorType?) {
                callback.error(error!!)
            }
        })
    }

    fun getLightness(callback: GetLightnessCallback) {
        controlElement.getStatus(LightningLightnessActual(), object : GetElementStatusCallback<LightningLightnessActual> {
            override fun success(element: Element?, group: Group?, value: LightningLightnessActual?) {
                callback.success(value!!.lightness)
            }

            override fun error(element: Element?, group: Group?, error: ErrorType?) {
                callback.error(error!!)
            }
        })
    }

    fun setLightness(lightnessPercentage: Int, callback: SetCallback) {
        val generic = LightningLightnessActual()
        generic.lightness = ControlConverters.getLightness(lightnessPercentage)
        controlElement.setStatus(generic, parameters, object : SetElementStatusCallback<LightningLightnessActual> {
            override fun success(element: Element?, group: Group?, value: LightningLightnessActual?) {
            }

            override fun error(element: Element?, group: Group?, error: ErrorType?) {
                callback.error(error!!)
            }
        })
    }

    fun getColorTemperature(callback: GetColorTemperatureCallback) {
        controlElement.getStatus(LightningCTLGet(), object : GetElementStatusCallback<LightningCTLGet> {
            override fun success(element: Element?, group: Group?, value: LightningCTLGet?) {
                callback.success(value!!.lightness, value.temperature)
            }

            override fun error(element: Element?, group: Group?, error: ErrorType?) {
                callback.error(error!!)
            }
        })
    }

    fun setColorTemperature(lightnessPercentage: Int, temperaturePercentage: Int, deltaUvPercentage: Int, callbackPercentage: SetCallback) {
        val generic = LightningCTLSet()
        generic.lightness = ControlConverters.getLightness(lightnessPercentage)
        generic.temperature = ControlConverters.getTemperature(temperaturePercentage)
        generic.deltaUv = ControlConverters.getDeltaUv(deltaUvPercentage)
        controlElement.setStatus(generic, parameters, object : SetElementStatusCallback<LightningCTLSet> {
            override fun success(element: Element?, group: Group?, value: LightningCTLSet?) {
            }

            override fun error(element: Element?, group: Group?, error: ErrorType?) {
                callbackPercentage.error(error!!)
            }
        })
    }

    fun getColorDeltaUv(callback: GetColorDeltaUvCallback) {
        controlElement.getStatus(LightningCTLTemperature(), object : GetElementStatusCallback<LightningCTLTemperature> {
            override fun success(element: Element?, group: Group?, value: LightningCTLTemperature?) {
                callback.success(value!!.temperature, value.deltaUv)
            }

            override fun error(element: Element?, group: Group?, error: ErrorType?) {
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