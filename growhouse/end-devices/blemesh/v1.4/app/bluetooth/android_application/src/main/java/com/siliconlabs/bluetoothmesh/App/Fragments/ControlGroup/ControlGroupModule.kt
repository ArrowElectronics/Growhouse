/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.ControlGroup

import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic
import com.siliconlabs.bluetoothmesh.App.Logic.NetworkConnectionLogic
import com.siliconlabs.bluetoothmesh.App.Models.MeshGroupControl
import com.siliconlabs.bluetoothmesh.App.Models.MeshNodeManager
import dagger.Module
import dagger.Provides

/**
 * @author Comarch S.A.
 */
@Module
class ControlGroupModule {
    @Provides
    fun provideControlGroupView(controlGroupFragment: ControlGroupFragment): ControlGroupView {
        return controlGroupFragment
    }

    @Provides
    fun provideControlGroupPresenter(controlGroupView: ControlGroupView, networkConnectionLogic: NetworkConnectionLogic, meshLogic: MeshLogic, meshNodeManager: MeshNodeManager): ControlGroupPresenter {
        return ControlGroupPresenter(controlGroupView, networkConnectionLogic, meshLogic, MeshGroupControl(meshLogic.currentGroup!!), meshNodeManager)
    }
}