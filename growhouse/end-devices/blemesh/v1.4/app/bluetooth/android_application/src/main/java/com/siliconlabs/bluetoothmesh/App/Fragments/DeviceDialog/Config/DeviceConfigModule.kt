/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.DeviceDialog.Config

import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic
import dagger.Module
import dagger.Provides

/**
 * @author Comarch S.A.
 */
@Module
class DeviceConfigModule {
    @Provides
    fun provideDeviceConfigView(deviceConfigFragment: DeviceConfigFragment): DeviceConfigView {
        return deviceConfigFragment
    }

    @Provides
    fun provideDeviceConfigPresenter(deviceConfigView: DeviceConfigView, meshLogic: MeshLogic): DeviceConfigPresenter {
        return DeviceConfigPresenter(deviceConfigView, meshLogic)
    }

}