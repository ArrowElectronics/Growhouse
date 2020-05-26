/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.DeviceDialog.Info

import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic
import dagger.Module
import dagger.Provides

/**
 * @author Comarch S.A.
 */
@Module
class DeviceInfoModule {
    @Provides
    fun provideDeviceInfoView(deviceInfoFragment: DeviceInfoFragment): DeviceInfoView {
        return deviceInfoFragment
    }

    @Provides
    fun provideDeviceInfoPresenter(deviceInfoView: DeviceInfoView, meshLogic: MeshLogic): DeviceInfoPresenter {
        return DeviceInfoPresenter(deviceInfoView, meshLogic)
    }
}