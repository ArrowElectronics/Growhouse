/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.Scanner

import android.os.Bundle
import android.support.v7.widget.LinearLayoutManager
import android.support.v7.widget.SimpleItemAnimator
import android.view.*
import android.widget.LinearLayout
import android.widget.Toast
import com.siliconlabs.bluetoothmesh.App.Models.DeviceDescription
import com.siliconlabs.bluetoothmesh.R
import dagger.android.support.DaggerFragment
import kotlinx.android.synthetic.main.scanner_fragment.*
import javax.inject.Inject

class ScannerFragment : DaggerFragment(), ScannerView {
    @Inject
    lateinit var scannerPresenter: ScannerPresenter

    private lateinit var adapter: ScannerAdapter
    private var scanMenu: MenuItem? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        adapter = ScannerAdapter(context!!, presenter = scannerPresenter)
        super.onCreate(savedInstanceState)
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        return inflater.inflate(R.layout.scanner_fragment, container, false)
    }

    override fun onResume() {
        super.onResume()
        scannerPresenter.onResume()
    }

    override fun onPause() {
        scannerPresenter.onPause()
        super.onPause()
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        setHasOptionsMenu(true)
        recyclerView.layoutManager = LinearLayoutManager(context, LinearLayout.VERTICAL, false)
        recyclerView.adapter = adapter
        (recyclerView.itemAnimator as SimpleItemAnimator).supportsChangeAnimations = false
        setEmptyHint()
    }

    override fun addDevice(deviceDescription: DeviceDescription) {
        activity?.runOnUiThread {
            adapter.addDevice(deviceDescription)
            setEmptyHint()
        }
    }

    override fun clearAdapter() {
        activity?.runOnUiThread {
            adapter.clearAdapter()
            setEmptyHint()
        }
    }

    override fun onCreateOptionsMenu(menu: Menu?, inflater: MenuInflater?) {
        inflater!!.inflate(R.menu.menu_scan_screen_toolbar, menu)
        scanMenu = menu!!.findItem(R.id.scan_menu)
        scanMenu?.setShowAsAction(MenuItem.SHOW_AS_ACTION_IF_ROOM)
        super.onCreateOptionsMenu(menu, inflater)
    }

    override fun onOptionsItemSelected(item: MenuItem?): Boolean {
        if (item!!.itemId == R.id.scan_menu) {
            scannerPresenter.startStopScanClicked()
            return true
        }
        return false
    }

    override fun setScanMenuToActive(active: Boolean) {
        activity?.runOnUiThread {
            scanMenu?.setTitle(if (active) R.string.device_scanner_turn_off_scan else R.string.device_scanner_turn_on_scan)
        }
    }

    override fun showToast(toastMessage: ScannerView.TOAST_MESSAGE) {
        activity?.runOnUiThread {
            Toast.makeText(context, R.string.scanner_adapter_no_network_on_provisioning, Toast.LENGTH_LONG).show()
        }
    }

    //

    private fun setEmptyHint() {
        activity?.runOnUiThread {
            if (adapter.list.isEmpty()) {
                ll_empty_view.visibility = View.VISIBLE
            } else {
                ll_empty_view.visibility = View.GONE
            }
        }
    }
}