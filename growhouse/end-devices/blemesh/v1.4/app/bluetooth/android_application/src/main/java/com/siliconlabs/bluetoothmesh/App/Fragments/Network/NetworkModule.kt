/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.Network

import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic
import com.siliconlabs.bluetoothmesh.App.Logic.NetworkConnectionLogic
import dagger.Module
import dagger.Provides

/**
 * @author Comarch S.A.
 */
@Module
class NetworkModule {
    @Provides
    fun provideNetworkView(networkFragment: NetworkFragment): NetworkView {
        return networkFragment
    }

    @Provides
    fun provideNetworkPresenter(networkView: NetworkView, networkConnectionLogic: NetworkConnectionLogic, meshLogic: MeshLogic): NetworkPresenter {
        return NetworkPresenter(networkView, networkConnectionLogic, meshLogic)
    }
}