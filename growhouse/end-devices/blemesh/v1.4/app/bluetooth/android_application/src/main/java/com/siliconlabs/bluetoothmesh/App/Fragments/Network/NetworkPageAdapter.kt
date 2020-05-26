/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.Network

import android.content.Context
import android.support.v4.app.Fragment
import android.support.v4.app.FragmentManager
import android.support.v4.app.FragmentStatePagerAdapter
import android.support.v4.view.PagerAdapter
import android.view.ViewGroup
import com.siliconlabs.bluetoothmesh.App.Fragments.DeviceList.DeviceListFragment
import com.siliconlabs.bluetoothmesh.App.Fragments.GroupList.GroupListFragment
import com.siliconlabs.bluetoothmesh.R


/**
 * @author Comarch S.A.
 */
class NetworkPageAdapter(fm: FragmentManager, private val context: Context) : FragmentStatePagerAdapter(fm) {

    val fragments = hashMapOf<Int, Fragment>()

    override fun getItem(position: Int): Fragment {
        return when (position) {
            0 -> GroupListFragment()
            else -> DeviceListFragment()
        }
    }

    override fun getCount(): Int {
        return 2
    }

    override fun getPageTitle(position: Int): CharSequence {
        return when (position) {
            0 -> context.getString(R.string.network_fragment_groups_page_title)
            else -> context.getString(R.string.network_fragment_devices_page_title)
        }
    }

    override fun getItemPosition(`object`: Any): Int {
        return PagerAdapter.POSITION_NONE
    }

    override fun instantiateItem(container: ViewGroup, position: Int): Any {
        val fragment = super.instantiateItem(container, position)
        fragments[position] = fragment as Fragment
        return fragment
    }

    override fun destroyItem(container: ViewGroup, position: Int, `object`: Any) {
        super.destroyItem(container, position, `object`)
        fragments.remove(position)
    }

    fun getFragment(position: Int): Fragment? {
        return fragments[position]
    }
}