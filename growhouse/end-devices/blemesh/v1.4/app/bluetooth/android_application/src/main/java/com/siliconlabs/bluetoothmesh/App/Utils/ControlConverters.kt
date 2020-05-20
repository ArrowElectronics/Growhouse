/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Utils

class ControlConverters {
    companion object {
        private var INT_16_MIN = -32768
        private var INT_16_MAX = 32767

        private var UINT_16_MIN = 0
        private var UINT_16_MAX = 65535

        private var TEMPERATURE_MIN = 800
        private var TEMPERATURE_MAX = 20000

        private var DELTA_UV_MIN = -1.00f
        private var DELTA_UV_MAX = 1.00f

        fun getLevel(percentage: Int): Int {
            val percentageDouble = percentage.toDouble() / 100
            var levelValue = percentageDouble * UINT_16_MAX
            levelValue += INT_16_MIN
            return Math.ceil(levelValue).toInt()
        }

        fun getLightness(percentage: Int): Int {
            val percentageDouble = percentage.toDouble() / 100
            return Math.ceil(percentageDouble * UINT_16_MAX).toInt()
        }

        fun getTemperature(percentage: Int): Int {
            val percentageDouble = percentage.toDouble() / 100
            var temperatureValue = percentageDouble * (TEMPERATURE_MAX - TEMPERATURE_MIN)
            temperatureValue += TEMPERATURE_MIN
            return Math.ceil(temperatureValue).toInt()
        }

        fun getDeltaUv(percentage: Int): Int {
            val percentageDouble = percentage.toDouble() / 100
            var deltaUv = percentageDouble * UINT_16_MAX
            deltaUv += INT_16_MIN
            return Math.ceil(deltaUv).toInt()
        }

        fun getDeltaUvToShow(percentage: Int): Float {
            return (percentage * 2 * DELTA_UV_MAX / 100) + DELTA_UV_MIN
        }
    }
}