/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.GroupList

import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic
import com.siliconlabs.bluetoothmesh.App.Models.MeshNetworkManager
import dagger.Module
import dagger.Provides

/**
 * @author Comarch S.A.
 */
@Module
class GroupListModule {

    @Provides
    fun provideGroupListView(groupListFragment: GroupListFragment): GroupListView {
        return groupListFragment
    }

    @Provides
    fun provideGroupListPresenter(groupListView: GroupListView, meshLogic: MeshLogic, meshNetworkManager: MeshNetworkManager): GroupListPresenter {
        return GroupListPresenter(groupListView, meshLogic, meshNetworkManager)
    }

}