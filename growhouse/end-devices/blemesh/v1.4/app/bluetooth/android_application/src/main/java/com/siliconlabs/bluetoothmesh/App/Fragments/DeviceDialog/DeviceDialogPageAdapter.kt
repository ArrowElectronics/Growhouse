/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.DeviceDialog

import android.content.Context
import android.support.v4.app.Fragment
import android.support.v4.app.FragmentManager
import android.support.v4.app.FragmentPagerAdapter
import com.siliconlabs.bluetoothmesh.App.Fragments.DeviceDialog.Config.DeviceConfigFragment
import com.siliconlabs.bluetoothmesh.App.Fragments.DeviceDialog.Info.DeviceInfoFragment
import com.siliconlabs.bluetoothmesh.R

/**
 * @author Comarch S.A.
 */
class DeviceDialogPageAdapter(fm: FragmentManager, private val context: Context) : FragmentPagerAdapter(fm){

    override fun getItem(position: Int): Fragment {
        return when (position) {
            0 -> DeviceConfigFragment()
            else -> DeviceInfoFragment()
        }
    }

    override fun getCount(): Int {
        return 2
    }

    override fun getPageTitle(position: Int): CharSequence {
        return when (position) {
            0 -> context.getString(R.string.device_dialog_config_page_title)
            else -> context.getString(R.string.device_dialog_info_page_title)
        }
    }

}