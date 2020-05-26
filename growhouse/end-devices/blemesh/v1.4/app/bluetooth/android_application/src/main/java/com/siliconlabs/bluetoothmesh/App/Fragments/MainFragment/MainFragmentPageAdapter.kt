/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.MainFragment

import android.content.Context
import android.support.v4.app.Fragment
import android.support.v4.app.FragmentManager
import android.support.v4.app.FragmentStatePagerAdapter
import com.siliconlabs.bluetoothmesh.App.Fragments.NetworkList.NetworkListFragment
import com.siliconlabs.bluetoothmesh.App.Fragments.Scanner.ScannerFragment
import com.siliconlabs.bluetoothmesh.R

/**
 * @author Comarch S.A.
 */
class MainFragmentPageAdapter(fm: FragmentManager, private val context: Context) : FragmentStatePagerAdapter(fm) {
    private val TAG: String = javaClass.canonicalName

    override fun getItem(position: Int): Fragment {
        return when (position) {
            0 -> NetworkListFragment()
            else -> ScannerFragment()
        }
    }

    override fun getCount(): Int {
        return 2
    }

    override fun getPageTitle(position: Int): CharSequence {
        return when (position) {
            0 -> context.getString(R.string.main_activity_networks_page_title)
            else -> context.getString(R.string.main_activity_provision_page_title)
        }
    }
}