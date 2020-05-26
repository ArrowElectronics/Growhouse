/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.ControlGroup

import android.os.Bundle
import android.view.*
import android.view.animation.Animation
import android.view.animation.AnimationUtils
import android.widget.ImageView
import android.widget.ListView
import android.widget.SeekBar
import android.widget.Toast
import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlabs.bluetoothmesh.App.Activities.Main.MainActivity
import com.siliconlabs.bluetoothmesh.App.Dialogs.DeviceEditionDialogs
import com.siliconlabs.bluetoothmesh.App.Dialogs.DeviceEditionDialogsPresenter
import com.siliconlabs.bluetoothmesh.App.Fragments.DeviceList.DeviceListAdapter
import com.siliconlabs.bluetoothmesh.App.Fragments.DeviceList.DeviceListView
import com.siliconlabs.bluetoothmesh.App.Fragments.Network.NetworkView
import com.siliconlabs.bluetoothmesh.App.ModelView.MeshNode
import com.siliconlabs.bluetoothmesh.App.Utils.ErrorMessageConverter
import com.siliconlabs.bluetoothmesh.R
import dagger.android.support.DaggerFragment
import kotlinx.android.synthetic.main.control_group.*
import javax.inject.Inject

/**
 * @author Comarch S.A.
 */
class ControlGroupFragment : DaggerFragment(), ControlGroupView, DeviceEditionDialogs.ActivityProvider, DeviceEditionDialogsPresenter.ParentView {
    private val TAG: String = javaClass.canonicalName

    override fun refreshList() {
        activity?.runOnUiThread {
            controlGroupPresenter.refreshList()
        }
    }

    override fun returnToNetworkList() {
        val mainActivity = activity as MainActivity
        mainActivity.returnToNetworkListFragment()
    }

    @Inject
    lateinit var controlGroupPresenter: ControlGroupPresenter

    private var devicesListAdapter: DeviceListAdapter? = null

    private lateinit var rotate: Animation
    private var meshStatusBtn: ImageView? = null
    private var meshIconStatus = ControlGroupView.MESH_ICON_STATE.DISCONNECTED
    lateinit var deviceEditionDialogsPresenter: DeviceEditionDialogsPresenter
    lateinit var deviceEditionDialogs: DeviceEditionDialogs

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        rotate = AnimationUtils.loadAnimation(context, R.anim.rotate)
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        return inflater.inflate(R.layout.control_group, container, false)
    }

    var switchEnabled = false

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        deviceEditionDialogs = DeviceEditionDialogs(this, controlGroupPresenter.meshLogic)
        deviceEditionDialogsPresenter = DeviceEditionDialogsPresenter(deviceEditionDialogs, this, controlGroupPresenter.meshLogic, controlGroupPresenter.meshNodeManager)
        deviceEditionDialogs.deviceEditionDialogsListener = deviceEditionDialogsPresenter
        setHasOptionsMenu(true)

        devicesListAdapter = DeviceListAdapter(context!!, controlGroupPresenter.networkConnectionLogic, controlGroupPresenter)
        devices_list.adapter = devicesListAdapter
        devices_list.choiceMode = ListView.CHOICE_MODE_MULTIPLE_MODAL
        devices_list.setMultiChoiceModeListener(devicesListAdapter)
        devices_list.emptyView = ll_empty_view

        iv_switch.setOnClickListener {
            switchEnabled = !switchEnabled
            controlGroupPresenter.onMasterSwitchChanged(switchEnabled)
        }

        sb_light_control.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
                seekBar?.let {
                    controlGroupPresenter.onMasterLevelChanged(it.progress)
                }
            }
        })
    }

    override fun onCreateOptionsMenu(menu: Menu?, inflater: MenuInflater?) {
        super.onCreateOptionsMenu(menu, inflater)
        inflater?.inflate(R.menu.menu_control_group_toolbar, menu)

        val menuIcon = menu?.findItem(R.id.proxy_menu)

        meshStatusBtn?.clearAnimation()
        meshStatusBtn?.visibility = View.INVISIBLE
        meshStatusBtn?.setOnClickListener(null)

        meshStatusBtn = menuIcon?.actionView as ImageView

        setMeshIconState(meshIconStatus)
    }

    override fun onResume() {
        super.onResume()
        (activity as MainActivity).setActionBar(controlGroupPresenter.groupInfo.name)
        controlGroupPresenter.onResume()
    }

    override fun onPause() {
        super.onPause()
        controlGroupPresenter.onPause()
        meshStatusBtn?.clearAnimation()
        devicesListAdapter?.finishActionMode()
    }

    override fun setUserVisibleHint(isVisibleToUser: Boolean) {
        super.setUserVisibleHint(isVisibleToUser)
        if (!isVisibleToUser) {
            devicesListAdapter?.finishActionMode()
        }
    }

    override fun setMeshIconState(iconState: ControlGroupView.MESH_ICON_STATE) {
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
                controlGroupPresenter.meshIconClicked(iconState)
            }
        }
    }

    override fun setMasterSwitch(isChecked: Boolean) {
        if (isChecked) {
            iv_switch.setImageResource(R.drawable.toggle_on)
        } else {
            iv_switch.setImageResource(R.drawable.toggle_off)
        }
    }

    override fun showErrorToast(message: String) {
        Toast.makeText(context, message, Toast.LENGTH_LONG).show()
    }

    override fun showErrorToast(errorType: ErrorType) {
        showErrorToast(ErrorMessageConverter.convert(context!!, errorType))
    }

    override fun setMasterLevel(progress: Int) {
        sb_light_control.progress = progress
        tv_light_value.text = context!!.getString(R.string.device_adapter_lightness_value).format(progress)
    }


    override fun setMasterControlEnabled(enabled: Boolean) {
        val alpha = if (enabled) 1f else 0.5f

        iv_switch.isEnabled = enabled
        iv_switch.alpha = alpha
        sb_light_control.isEnabled = enabled
        sb_light_control.alpha = alpha
    }

    override fun setMasterControlVisibility(visibility: Int) {
        ll_master_control.visibility = visibility
    }

    override fun refreshView() {
        devicesListAdapter?.notifyDataSetChanged()
    }

    override fun setDevicesList(devicesInfo: Set<MeshNode>) {
        devicesListAdapter?.clear()
        devicesListAdapter?.addAll(devicesInfo.toMutableList())
        devicesListAdapter?.notifyDataSetChanged()
    }

    override fun notifyItemChanged(deviceInfo: MeshNode) {
        devicesListAdapter?.notifyDataSetChanged()
    }

    override fun showToast(message: DeviceEditionDialogsPresenter.TOAST_MESSAGE) {
        activity?.runOnUiThread {
            val stringMessage: String? = when (message) {
                DeviceEditionDialogsPresenter.TOAST_MESSAGE.ERROR_DELETE_DEVICE -> context!!.getString(R.string.device_adapter_remove_device_error)
                DeviceEditionDialogsPresenter.TOAST_MESSAGE.ERROR_MISSING_GROUP-> "Missing group"
            }

            stringMessage?.let {
                Toast.makeText(context, stringMessage, Toast.LENGTH_SHORT).show()
            }
        }
    }

    override fun showToast(toastMessage: ControlGroupView.TOAST_MESSAGE) {
        val message: String? = when (toastMessage) {
            ControlGroupView.TOAST_MESSAGE.NOT_CONNECTED_TO_MESH_NETWORK -> context!!.getString(R.string.control_group_not_connected_to_mesh_network)
            ControlGroupView.TOAST_MESSAGE.TO_DO -> "TO DO"
        }

        message?.let {
            Toast.makeText(context, message, Toast.LENGTH_SHORT).show()
        }
    }


    override fun showDeleteDeviceDialog(deviceInfo: List<MeshNode>) {
        deviceEditionDialogs.showDeleteDeviceDialog(deviceInfo)
        devicesListAdapter?.finishActionMode()
    }

    override fun showDeviceConfigDialog(deviceInfo: MeshNode) {
        deviceEditionDialogsPresenter.showDeviceConfigDialog(deviceInfo)
        devicesListAdapter?.finishActionMode()
    }

    override fun showLoadingDialog() {
        deviceEditionDialogs.showLoadingDialog()
    }

    override fun updateLoadingDialogMessage(loadingMessage: DeviceListView.LOADING_DIALOG_MESSAGE, errorCode: String, showCloseButton: Boolean) {
        deviceEditionDialogs.updateLoadingDialogMessage(loadingMessage, errorCode, showCloseButton)
    }

}