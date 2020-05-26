/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.Network

import android.os.Bundle
import android.view.*
import android.view.animation.Animation
import android.view.animation.AnimationUtils
import android.widget.ImageView
import android.widget.Toast
import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlabs.bluetoothmesh.App.Activities.Main.MainActivity
import com.siliconlabs.bluetoothmesh.App.Fragments.DeviceList.DeviceListFragment
import com.siliconlabs.bluetoothmesh.App.Fragments.GroupList.GroupListFragment
import com.siliconlabs.bluetoothmesh.App.ModelView.MeshNode
import com.siliconlabs.bluetoothmesh.App.Utils.ErrorMessageConverter
import com.siliconlabs.bluetoothmesh.R
import dagger.android.support.DaggerFragment
import kotlinx.android.synthetic.main.network_fragment_screen.*
import javax.inject.Inject


/**
 * @author Comarch S.A.
 */
class NetworkFragment : DaggerFragment(), NetworkView {
    private val TAG: String = javaClass.canonicalName

    private lateinit var rotate: Animation
    private var meshStatusBtn: ImageView? = null
    private var meshIconStatus = NetworkView.MESH_ICON_STATE.DISCONNECTED

    private var pageAdapter: NetworkPageAdapter? = null

    @Inject
    lateinit var networkPresenter: NetworkPresenter

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        rotate = AnimationUtils.loadAnimation(context, R.anim.rotate)
    }

    override fun onResume() {
        super.onResume()
        (activity as MainActivity).supportActionBar?.setDisplayHomeAsUpEnabled(true)
        networkPresenter.onResume()
    }

    override fun onPause() {
        super.onPause()
        networkPresenter.onPause()
        meshStatusBtn?.clearAnimation()
    }

    override fun onDestroy() {
        super.onDestroy()
        networkPresenter.onDestroy()
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        return inflater.inflate(R.layout.network_fragment_screen, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        setHasOptionsMenu(true)

        pageAdapter = NetworkPageAdapter(childFragmentManager, context!!)
        view_pager.adapter = pageAdapter

        tab_layout.setupWithViewPager(view_pager)
    }

    override fun onCreateOptionsMenu(menu: Menu?, inflater: MenuInflater?) {
        super.onCreateOptionsMenu(menu, inflater)
        inflater?.inflate(R.menu.menu_groups_toolbar, menu)

        val menuIcon = menu?.findItem(R.id.proxy_menu)

        meshStatusBtn?.clearAnimation()
        meshStatusBtn?.visibility = View.INVISIBLE
        meshStatusBtn?.setOnClickListener(null)

        meshStatusBtn = menuIcon?.actionView as ImageView

        setMeshIconState(meshIconStatus)
    }

    override fun onOptionsItemSelected(item: MenuItem?): Boolean {
        if (item?.itemId == R.id.proxy_menu) {
            return true
        }

        return super.onOptionsItemSelected(item)
    }

    // View

    override fun showFragment(fragment: NetworkView.FRAGMENT) {
        view_pager.currentItem = fragment.ordinal
    }

    override fun setActionBarTitle(title: String) {
        (activity as MainActivity).setActionBar(title)
    }

    override fun setMeshIconState(iconState: NetworkView.MESH_ICON_STATE) {
        activity?.runOnUiThread {
            meshIconStatus = iconState

            meshStatusBtn?.apply {
                when (iconState) {
                    NetworkView.MESH_ICON_STATE.DISCONNECTED -> {
                        setImageResource(R.drawable.ic_mesh_red)
                        clearAnimation()
                    }
                    NetworkView.MESH_ICON_STATE.CONNECTING -> {
                        setImageResource(R.drawable.ic_mesh_yellow)
                        startAnimation(rotate)
                    }
                    NetworkView.MESH_ICON_STATE.CONNECTED -> {
                        setImageResource(R.drawable.ic_mesh_green)
                        clearAnimation()
                    }
                }

                setOnClickListener {
                    networkPresenter.meshIconClicked(iconState)
                }
            }
        }
    }

    override fun showToast(toastMessage: NetworkView.TOAST_MESSAGE) {
        activity?.runOnUiThread {
            val message: String? = when (toastMessage) {
                NetworkView.TOAST_MESSAGE.NO_NODE_IN_NETWORK -> getString(R.string.network_fragment_toast_no_node_in_network)
                NetworkView.TOAST_MESSAGE.NO_PROXY_NODE_IN_NETWORK -> getString(R.string.network_fragment_toast_no_proxy_node_in_network)

                NetworkView.TOAST_MESSAGE.MESH_NETWORK_DISCONNECTED -> getString(R.string.network_fragment_mesh_network_disconnected)
                NetworkView.TOAST_MESSAGE.GATT_NOT_CONNECTED -> getString(R.string.network_fragment_toast_gatt_not_connected)
                NetworkView.TOAST_MESSAGE.GATT_PROXY_DISCONNECTED -> getString(R.string.network_fragment_gatt_proxy_disconnected)
                NetworkView.TOAST_MESSAGE.GATT_ERROR_DISCOVERING_SERVICES -> getString(R.string.network_fragment_gatt_error_discovering_services)

                NetworkView.TOAST_MESSAGE.PROXY_SERVICE_NOT_FOUND -> getString(R.string.network_fragment_toast_no_mesh_proxy_service)
                NetworkView.TOAST_MESSAGE.PROXY_CHARACTERISTIC_NOT_FOUND -> getString(R.string.network_fragment_toast_no_mesh_proxy_characteristic)
                NetworkView.TOAST_MESSAGE.PROXY_DESCRIPTOR_NOT_FOUND -> getString(R.string.network_fragment_toast_no_mesh_proxy_descriptor)

                NetworkView.TOAST_MESSAGE.CONNECTION_TIMEOUT -> getString(R.string.network_fragment_toast_connection_timeout)
            }

            message?.let {
                Toast.makeText(activity, message, Toast.LENGTH_SHORT).show()
            }
        }
    }

    override fun showErrorToast(errorType: ErrorType) {
        activity?.runOnUiThread {
            Toast.makeText(activity, ErrorMessageConverter.convert(activity!!, errorType), Toast.LENGTH_LONG).show()
        }
    }

    override fun showConfigDeviceFragment(deviceInfo: MeshNode) {
        view_pager.currentItem = 2

        val devicesFragment: DeviceListFragment? = pageAdapter?.instantiateItem(view_pager, 1) as DeviceListFragment
        devicesFragment?.dismissLoadingDialog()
        devicesFragment?.deviceListPresenter?.startConfigDevice(deviceInfo)
    }

    fun refreshFragment(fragment: NetworkView.FRAGMENT) {
        val fragmentToRefresh = pageAdapter?.getFragment(fragment.ordinal)
        fragmentToRefresh?.let {
            if (!it.isResumed) {
                return
            }
            when (fragment) {
                NetworkView.FRAGMENT.GROUP_LIST -> {
                    (it as GroupListFragment).onResume()
                }
                NetworkView.FRAGMENT.DEVICE_LIST -> {
                    (it as DeviceListFragment).onResume()
                }
            }
        }

    }
}