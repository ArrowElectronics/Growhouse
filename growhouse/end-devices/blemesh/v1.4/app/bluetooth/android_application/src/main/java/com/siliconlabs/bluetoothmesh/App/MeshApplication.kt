/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App

import android.app.Activity
import android.app.Application
import com.siliconlabs.bluetoothmesh.App.Utils.LogUtils
import dagger.android.DispatchingAndroidInjector
import dagger.android.HasActivityInjector
import javax.inject.Inject


/**
 * @author Comarch S.A.
 */
class MeshApplication : Application(), HasActivityInjector {

    @Inject
    lateinit var activityDispatchingAndroidInjector: DispatchingAndroidInjector<Activity>


    override fun onCreate() {
        super.onCreate()

        LogUtils.setupLogcatLogs(applicationInfo)
        DaggerAppComponent.builder().application(this).build().inject(this)
    }

    override fun activityInjector(): DispatchingAndroidInjector<Activity> {
        return activityDispatchingAndroidInjector
    }
}