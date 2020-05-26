/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Models

import com.siliconlab.bluetoothmesh.adk.ErrorType

class ProvisioningStatusPropagator {
    private val provisioningStatusListeners = ArrayList<ProvisioningListener>()

    internal fun propagateProvisioningStatus(provisioningStatus: ProvisioningStatus, status: Int = 0) {
        provisioningStatusListeners.forEach { provisioningListener: ProvisioningListener -> provisioningListener.provisioningStatusChanged(provisioningStatus) }
    }

    internal fun propagateProvisioningError(provisioningStatus: ProvisioningStatus, error: ErrorType) {
        provisioningStatusListeners.forEach { provisioningListener: ProvisioningListener -> provisioningListener.provisioningUnsuccessful(provisioningStatus, error) }
    }

    fun addProvisioningStatusListener(provisioningListener: ProvisioningListener) {
        provisioningStatusListeners.add(provisioningListener)
    }

    fun removeProvisioningStatusListener(provisioningListener: ProvisioningListener) {
        provisioningStatusListeners.remove(provisioningListener)
    }

    enum class ProvisioningStatus {
        ProvisioningSuccessful,
        ProvisioningError,
        DeviceProvisionedGattDisconnected,
        DeviceAlreadyAdded,

        ErrorDuringProvisioning,
        ErrorRetrying,
    }

    interface ProvisioningListener {
        fun provisioningStatusChanged(provisioningStatus : ProvisioningStatus)
        fun provisioningUnsuccessful(provisioningStatus: ProvisioningStatus, error: ErrorType)
    }
}