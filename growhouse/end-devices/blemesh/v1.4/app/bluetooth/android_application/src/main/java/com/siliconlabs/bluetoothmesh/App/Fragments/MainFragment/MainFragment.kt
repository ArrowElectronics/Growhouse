/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.MainFragment

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.provider.Settings
import android.support.v4.content.ContextCompat
import android.support.v7.app.AlertDialog
import android.util.Log
import android.view.*
import com.siliconlabs.bluetoothmesh.App.Activities.Main.MainActivity
import com.siliconlabs.bluetoothmesh.BuildConfig
import com.siliconlabs.bluetoothmesh.R
import dagger.android.support.DaggerFragment
import kotlinx.android.synthetic.main.dialog_about.view.*
import kotlinx.android.synthetic.main.main_screen.*
import javax.inject.Inject


/**
 * @author Comarch S.A.
 */
class MainFragment : DaggerFragment(), MainFragmentView {

    private val TAG: String = javaClass.canonicalName

    private val PERMISSION_REQUEST_COARSE_LOCATION = 1

    private var showedLocationAlertDialog = false
    private var requestedLocationPermission = false

    @Inject
    lateinit var mainFragmentPresenter: MainFragmentPresenter

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        return inflater.inflate(R.layout.main_screen, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        setHasOptionsMenu(true)
    }

    override fun onResume() {
        super.onResume()
        (activity as MainActivity).supportActionBar?.setDisplayHomeAsUpEnabled(false)
        (activity as MainActivity).setActionBar()
        mainFragmentPresenter.onResume()
    }

    override fun onPause() {
        super.onPause()
        mainFragmentPresenter.onPause()
    }

    override fun onCreateOptionsMenu(menu: Menu?, inflater: MenuInflater?) {
        super.onCreateOptionsMenu(menu, inflater)

        inflater!!.inflate(R.menu.menu_main_screen_toolbar, menu)
    }

    override fun onOptionsItemSelected(item: MenuItem?): Boolean {
        when (item?.itemId) {
            R.id.about_bluetooth_mesh -> {
                showAboutDialog()
                return true
            }
        }
        return super.onOptionsItemSelected(item)
    }

    // View
    override fun setView() {
        setTabLayout()

        checkBTAdapter()
        checkGPS()

        setEnablingButtons()
    }

    fun showAboutDialog() {
        val view: View = LayoutInflater.from(context).inflate(R.layout.dialog_about, null)
        view.apply {
            var appVersionName = getString(R.string.dialog_about_app_version).format(BuildConfig.VERSION_NAME)

            if (BuildConfig.DEBUG && BuildConfig.BUILD_NUMBER != null) {
                appVersionName += "\n" + "Build Number  " + BuildConfig.BUILD_NUMBER
            }
            tv_app_version.text = appVersionName

            var adkVersionName = getString(R.string.dialog_about_adk_version).format(com.siliconlab.bluetoothmesh.adk.BuildConfig.VERSION_NAME)
            if (BuildConfig.DEBUG && com.siliconlab.bluetoothmesh.adk.BuildConfig.BUILD_NUMBER != null) {
                adkVersionName += "\n" + "Build Number  " + com.siliconlab.bluetoothmesh.adk.BuildConfig.BUILD_NUMBER
            }
            tv_adk_version.text = adkVersionName

        }
        val builder = android.app.AlertDialog.Builder(activity, R.style.AppTheme_Light_Dialog_Alert_Wrap)
        builder.apply {
            setView(view)
            setPositiveButton(R.string.dialog_positive_ok) { dialog, _ ->
                dialog.dismiss()
            }
        }

        val dialog = builder.create()
        dialog.apply {
            show()
        }
    }

    // Permission callback

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<String>, grantResults: IntArray) {
        when (requestCode) {
            PERMISSION_REQUEST_COARSE_LOCATION -> {
                if (grantResults.isEmpty() || grantResults[0] != PackageManager.PERMISSION_GRANTED) {
                    if (showedLocationAlertDialog) {
                        return
                    }
                    showedLocationAlertDialog = true

                    val builder = AlertDialog.Builder(context!!)
                    builder.setCancelable(false)
                    builder.setTitle(getString(R.string.main_activity_dialog_location_permission_not_granted_title))
                    builder.setMessage(getString(R.string.main_activity_dialog_location_permission_not_granted_message))
                    builder.setPositiveButton(android.R.string.ok) { _, _ ->
                        showedLocationAlertDialog = false
                    }
                    builder.show()
                } else if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    Log.d(TAG, "coarse location permission granted")

                    setView()
                }
            }
        }
    }

    //

    private fun setTabLayout() {
        view_pager.adapter = MainFragmentPageAdapter(childFragmentManager, context!!)
        tab_layout.setupWithViewPager(view_pager)
    }

    override fun setEnablingButtons() {
        if (!BluetoothAdapter.getDefaultAdapter().isEnabled) {
            bluetooth_enable.visibility = View.VISIBLE
            bluetooth_enable_btn.setOnClickListener {
                BluetoothAdapter.getDefaultAdapter().enable()
            }
        } else {
            bluetooth_enable.visibility = View.GONE
        }

        if (!isLocationEnabled(context!!)) {
            location_enable.visibility = View.VISIBLE
            location_enable_btn.setOnClickListener {
                location_enable.visibility = View.GONE
                val intent = Intent(
                        Settings.ACTION_LOCATION_SOURCE_SETTINGS)
                startActivity(intent)
                if (Build.VERSION.SDK_INT >= 23)
                    requestPermissions(arrayOf(Manifest.permission.ACCESS_COARSE_LOCATION),
                            PERMISSION_REQUEST_COARSE_LOCATION)
            }
        } else {
            location_enable.visibility = View.GONE
        }
    }

    private fun isLocationEnabled(context: Context): Boolean {
        return try {
            Settings.Secure.getInt(context.contentResolver, Settings.Secure.LOCATION_MODE) != Settings.Secure.LOCATION_MODE_OFF
        } catch (e: Settings.SettingNotFoundException) {
            e.printStackTrace()
            false
        }
    }

    private fun checkGPS() {
        if (requestedLocationPermission) {
            return
        }
        requestedLocationPermission = true

        if (Build.VERSION.SDK_INT >= 23 && ContextCompat.checkSelfPermission(context!!, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            requestPermissions(arrayOf(Manifest.permission.ACCESS_COARSE_LOCATION),
                    PERMISSION_REQUEST_COARSE_LOCATION)
        }
    }

    private fun checkBTAdapter() {
        if (!activity!!.packageManager.hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            val builder = AlertDialog.Builder(context!!)
            builder.setCancelable(false)
            builder.setTitle(getString(R.string.main_activity_dialog_not_support_ble_title))
            builder.setMessage(getString(R.string.main_activity_dialog_not_support_ble_message))
            builder.setPositiveButton(getString(R.string.main_activity_dialog_not_support_ble_positive_button)) { _, _ ->
                activity!!.finish()
            }
            builder.show()
        }
    }
}