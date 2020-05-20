/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.MainFragment

import com.siliconlabs.bluetoothmesh.App.Logic.BluetoothStateReceiver
import dagger.Module
import dagger.Provides

/**
 * @author Comarch S.A.
 */
@Module
class MainFragmentModule {
    @Provides
    fun provideMainFragmentView(mainFragment: MainFragment): MainFragmentView {
        return mainFragment
    }

    @Provides
    fun provideMainFragmentPresenter(mainFragmentView: MainFragmentView, bluetoothStateReceiver: BluetoothStateReceiver): MainFragmentPresenter {
        return MainFragmentPresenter(mainFragmentView, bluetoothStateReceiver)
    }
}