/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Activities.Main

import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic
import com.siliconlabs.bluetoothmesh.App.Logic.NetworkConnectionLogic
import dagger.Module
import dagger.Provides

/**
 * @author Comarch S.A.
 */
@Module
class MainActivityModule {
    @Provides
    fun provideMainActivityView(mainActivity: MainActivity): MainActivityView {
        return mainActivity
    }

    @Provides
    fun provideMainActivityPresenter(mainActivityView: MainActivityView, meshLogic: MeshLogic, networkConnectionLogic: NetworkConnectionLogic): MainActivityPresenter {
        return MainActivityPresenter(mainActivityView, meshLogic, networkConnectionLogic)
    }
}