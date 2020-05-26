/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Activities.Main

import android.Manifest
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Bundle
import android.support.v4.app.ActivityCompat
import android.support.v4.app.Fragment
import android.support.v4.app.FragmentManager
import android.support.v4.content.ContextCompat
import android.support.v7.widget.Toolbar
import android.util.Log
import android.view.MenuItem
import android.view.View
import android.view.Window
import com.siliconlabs.bluetoothmesh.App.Fragments.MainFragment.MainFragment
import com.siliconlabs.bluetoothmesh.R
import dagger.android.support.DaggerAppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.android.synthetic.main.scanscreen_toolbar.view.*
import javax.inject.Inject

/**
 * @author Comarch S.A.
 */
class MainActivity : DaggerAppCompatActivity(), MainActivityView {

    companion object {
        const val PERMISSIONS_REQUEST_CODE: Int = 12
    }

    private val TAG: String = javaClass.canonicalName

    @Inject
    lateinit var mainActivityPresenter: MainActivityPresenter

    override fun onCreate(savedInstanceState: Bundle?) {
        Log.d(TAG, "onCreate")
        requestWindowFeature(Window.FEATURE_NO_TITLE)
        super.onCreate(savedInstanceState)

        setContentView(R.layout.activity_main)

        showFragment(MainFragment(), false, false)

        checkPermissions()
    }

    private fun checkPermissions() {
        val reqPermissions = ArrayList<String>()
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            reqPermissions.add(Manifest.permission.WRITE_EXTERNAL_STORAGE)
        }
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            reqPermissions.add(Manifest.permission.ACCESS_FINE_LOCATION)
        }
        if (reqPermissions.isNotEmpty()) {
            ActivityCompat.requestPermissions(this, reqPermissions.toTypedArray(), PERMISSIONS_REQUEST_CODE)
        }
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<String>, grantResults: IntArray) {
        when (requestCode) {
            PERMISSIONS_REQUEST_CODE -> {
                if ((grantResults.isNotEmpty())) {
                    grantResults.forEach { result ->
                        if (result != PackageManager.PERMISSION_GRANTED) {
                            finish()
                        }
                    }
                }
            }
        }
    }

    override fun onResume() {
        Log.d(TAG, "onResume")
        super.onResume()

        mainActivityPresenter.onResume()
    }

    override fun onPause() {
        Log.d(TAG, "onPause")
        super.onPause()

        mainActivityPresenter.onPause()
    }

    override fun onOptionsItemSelected(item: MenuItem?): Boolean {
        if (supportFragmentManager.backStackEntryCount > 0) {
            if (item?.itemId == android.R.id.home) {
                onBackPressed()
                return true
            }
        }
        return super.onOptionsItemSelected(item)
    }

    fun showFragment(fragment: Fragment, addToBackStack: Boolean, animated: Boolean) {
        val transaction = supportFragmentManager.beginTransaction()
        if (animated) {
            transaction.setCustomAnimations(R.anim.enter, R.anim.exit, R.anim.pop_enter, R.anim.pop_exit)
        }
        if (addToBackStack) {
            transaction.addToBackStack(null)
        }
        transaction.replace(R.id.fragment_container, fragment)
        transaction.commit()
    }

    fun returnToNetworkListFragment() {
        supportFragmentManager.popBackStack(null, FragmentManager.POP_BACK_STACK_INCLUSIVE)
    }

    // View


    override fun setActionBar(title: String?) {
        setSupportActionBar(toolbar as Toolbar)
        if (title == null) {
            toolbar.logo_action_bar.visibility = View.VISIBLE
            supportActionBar?.setDisplayShowTitleEnabled(false)
        } else {
            toolbar.logo_action_bar.visibility = View.GONE
            supportActionBar?.setDisplayShowTitleEnabled(true)
            supportActionBar?.setTitle(title)
        }
    }

    override fun exportMeshData(shareIntent: Intent) {
        startActivity(Intent.createChooser(shareIntent, "Export Mesh Network Keys"))
    }

}