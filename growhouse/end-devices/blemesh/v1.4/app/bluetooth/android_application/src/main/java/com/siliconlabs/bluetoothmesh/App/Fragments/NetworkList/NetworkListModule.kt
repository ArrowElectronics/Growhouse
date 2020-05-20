/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.NetworkList

import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic
import com.siliconlabs.bluetoothmesh.App.Logic.NetworkConnectionLogic
import com.siliconlabs.bluetoothmesh.App.Models.MeshNetworkManager
import com.siliconlabs.bluetoothmesh.App.Models.MeshNodeManager
import dagger.Module
import dagger.Provides

/**
 * @author Comarch S.A.
 */
@Module
class NetworkListModule {

    @Provides
    fun provideNetworkListView(networkListFragment: NetworkListFragment): NetworkListView {
        return networkListFragment
    }

    @Provides
    fun provideNetworkListPresenter(networkListView: NetworkListView, meshLogic: MeshLogic, meshNetworkManager: MeshNetworkManager, networkConnectionLogic: NetworkConnectionLogic, meshNodeManager: MeshNodeManager): NetworkListPresenter {
        return NetworkListPresenter(networkListView, meshLogic, meshNetworkManager, networkConnectionLogic, meshNodeManager)
    }

}