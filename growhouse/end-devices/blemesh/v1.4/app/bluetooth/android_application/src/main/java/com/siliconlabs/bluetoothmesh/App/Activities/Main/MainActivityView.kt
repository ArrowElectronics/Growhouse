/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Activities.Main

import android.content.Intent

/**
 * @author Comarch S.A.
 */
interface MainActivityView {

    fun exportMeshData(shareIntent: Intent)

    fun setActionBar(title: String? = null)

}