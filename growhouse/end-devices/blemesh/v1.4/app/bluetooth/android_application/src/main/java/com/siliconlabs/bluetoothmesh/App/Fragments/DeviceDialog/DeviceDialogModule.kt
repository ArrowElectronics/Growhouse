/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.DeviceDialog

import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic
import com.siliconlabs.bluetoothmesh.App.Logic.NetworkConnectionLogic
import dagger.Module
import dagger.Provides

/**
 * @author Comarch S.A.
 */
@Module
class DeviceDialogModule {
    @Provides
    fun provideDeviceDialogView(deviceDialogFragment: DeviceDialogFragment): DeviceDialogView {
        return deviceDialogFragment
    }

    @Provides
    fun provideDeviceDialogPresenter(deviceDialogView: DeviceDialogView, networkConnectionLogic: NetworkConnectionLogic, meshLogic: MeshLogic): DeviceDialogPresenter {
        return DeviceDialogPresenter(deviceDialogView, networkConnectionLogic, meshLogic)
    }
}