/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Activities

import android.app.Activity
import android.content.Intent
import android.os.Bundle
import com.siliconlabs.bluetoothmesh.App.Activities.Main.MainActivity

/**
 * @author Comarch S.A.
 */
class SplashActivity : Activity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val intent = Intent(this, MainActivity::class.java)
        startActivity(intent)
        finish()
    }
}