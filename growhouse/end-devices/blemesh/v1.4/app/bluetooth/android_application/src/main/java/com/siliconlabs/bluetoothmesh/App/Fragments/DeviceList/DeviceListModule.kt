/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.DeviceList

import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic
import com.siliconlabs.bluetoothmesh.App.Logic.NetworkConnectionLogic
import com.siliconlabs.bluetoothmesh.App.Models.MeshNodeManager
import dagger.Module
import dagger.Provides

/**
 * @author Comarch S.A.
 */
@Module
class DeviceListModule {

    @Provides
    fun provideDeviceListView(deviceListFragment: DeviceListFragment): DeviceListView {
        return deviceListFragment
    }

    @Provides
    fun provideDeviceListPresenter(deviceListView: DeviceListView, meshLogic: MeshLogic, networkConnectionLogic: NetworkConnectionLogic, meshNodeManager: MeshNodeManager): DeviceListPresenter {
        return DeviceListPresenter(deviceListView, meshLogic, networkConnectionLogic, meshNodeManager)
    }
}