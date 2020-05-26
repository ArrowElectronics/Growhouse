/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.DeviceDialog.Config

import android.app.AlertDialog
import android.os.Bundle
import android.text.Editable
import android.text.TextWatcher
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.view.WindowManager
import android.widget.AdapterView
import android.widget.ArrayAdapter
import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlab.bluetoothmesh.adk.data_model.group.Group
import com.siliconlabs.bluetoothmesh.App.Fragments.DeviceDialog.DeviceDialogFragment
import com.siliconlabs.bluetoothmesh.App.ModelView.MeshNode
import com.siliconlabs.bluetoothmesh.App.Models.DeviceConfig
import com.siliconlabs.bluetoothmesh.App.Models.DeviceFunctionality
import com.siliconlabs.bluetoothmesh.App.Utils.ErrorMessageConverter
import com.siliconlabs.bluetoothmesh.App.Views.CustomAlertDialogBuilder
import com.siliconlabs.bluetoothmesh.R
import dagger.android.support.DaggerFragment
import kotlinx.android.synthetic.main.dialog_device_config.*
import kotlinx.android.synthetic.main.dialog_loading.*
import javax.inject.Inject

/**
 * @author Comarch S.A.
 */
class DeviceConfigFragment : DaggerFragment(), DeviceConfigView, DeviceConfigListener {
    @Inject
    lateinit var deviceConfigPresenter: DeviceConfigPresenter

    private var loadingDialog: AlertDialog? = null

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        return inflater.inflate(R.layout.dialog_device_config, container, false)
    }

    override fun onResume() {
        super.onResume()
        deviceConfigPresenter.onResume()

        getDeviceDialogFragment().deviceDialogPresenter.deviceConfigListener = this
    }

    override fun onPause() {
        super.onPause()
        deviceConfigPresenter.onPause()

        getDeviceDialogFragment().deviceDialogPresenter.deviceConfigListener = null
    }

    override fun setDeviceConfig(deviceInfo: MeshNode, deviceConfig: DeviceConfig, groupsInSubnet: List<Group>) {
        deviceInfo.apply {
            device_name.setText(node.name)
            device_name.setSelection(device_name.text.length)
            device_name.addTextChangedListener(object : TextWatcher {
                override fun afterTextChanged(s: Editable?) {
                    deviceConfigPresenter.deviceConfig.name = s.toString()
                }

                override fun beforeTextChanged(s: CharSequence?, start: Int, count: Int, after: Int) {
                }

                override fun onTextChanged(s: CharSequence?, start: Int, before: Int, count: Int) {
                }
            })

            node.deviceCompositionData?.apply {
                sw_proxy.visibility = if (supportsProxy()) View.VISIBLE else View.GONE
                sw_relay.visibility = if (supportsRelay()) View.VISIBLE else View.GONE
                sw_friend.visibility = if (supportsFriend()) View.VISIBLE else View.GONE
                low_power_container.visibility = if (supportsLowPower()) View.VISIBLE else View.GONE
            }

            sw_proxy.isChecked = deviceConfig.proxy!!
            sw_relay.isChecked = deviceConfig.relay!!
            sw_friend.isChecked = deviceConfig.friend!!

            sw_proxy.setOnCheckedChangeListener { _, isChecked ->
                deviceConfigPresenter.deviceConfig.proxy = isChecked
            }
            sw_relay.setOnCheckedChangeListener { _, isChecked ->
                deviceConfigPresenter.deviceConfig.relay = isChecked
            }
            sw_friend.setOnCheckedChangeListener { _, isChecked ->
                deviceConfigPresenter.deviceConfig.friend = isChecked
            }

            val groupList = ArrayList<String>()
            groupsInSubnet.forEach { groupInfo ->
                groupList.add(groupInfo.name)
            }
            val groupAdapter = ArrayAdapter<String>(context, R.layout.spinner_item, groupList)

            sp_group.adapter = groupAdapter
            if (node.groups.isNotEmpty()) {
                val groupInfo = groupsInSubnet.find { group ->
                    group == node.groups.iterator().next()
                }
                groupInfo?.apply {
                    sp_group.setSelection(groupList.indexOf(name), false)
                }
            } else if (groupsInSubnet.isNotEmpty()) {
                sp_group.setSelection(0, false)
                deviceConfigPresenter.deviceConfig.group = groupsInSubnet.iterator().next()
            }
            sp_group.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
                override fun onItemSelected(parent: AdapterView<*>?, view: View?, position: Int, id: Long) {
                    deviceConfigPresenter.deviceConfig.group = groupsInSubnet[position]
                }

                override fun onNothingSelected(parent: AdapterView<*>?) {
                }
            }

            val functionalityList = DeviceFunctionality.getFunctionalities(node).toMutableList()
            functionalityList.sortBy { it.functionalityName }

            val typeList = ArrayList<String>()
            functionalityList.forEach {
                typeList.add(it.functionalityName)
            }

            val typeAdapter = ArrayAdapter<String>(context, R.layout.spinner_item, typeList)

            sp_functionality.adapter = typeAdapter
            if (functionality != DeviceFunctionality.FUNCTIONALITY.Unknown) {
                sp_functionality.setSelection(typeList.indexOf(functionality.functionalityName), false)
            } else {
                sp_functionality.setSelection(0, false)
            }
            sp_functionality.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
                override fun onItemSelected(parent: AdapterView<*>?, view: View?, position: Int, id: Long) {
                    deviceConfigPresenter.deviceConfig.functionality = functionalityList[position]
                }

                override fun onNothingSelected(parent: AdapterView<*>?) {
                }
            }
        }
    }

    private fun getDeviceDialogFragment(): DeviceDialogFragment {
        return parentFragment as DeviceDialogFragment
    }

    override fun showLoadingDialog() {
        activity?.runOnUiThread {
            val view: View = LayoutInflater.from(activity).inflate(R.layout.dialog_loading, null)
            val builder = CustomAlertDialogBuilder(activity!!, R.style.AppTheme_Light_Dialog_Alert_Wrap)
            builder.apply {
                setView(view)
                setCancelable(false)
                setPositiveButton(activity!!.getString(R.string.dialog_positive_ok)) { dialog, _ ->
                    getDeviceDialogFragment().dismiss()
                }
            }

            loadingDialog = builder.create()
            loadingDialog?.apply {
                window.setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE)
                show()

                getButton(AlertDialog.BUTTON_POSITIVE).visibility = View.GONE
            }
        }
    }

    override fun setLoadingDialogMessage(message: String, showCloseButton: Boolean) {
        activity?.runOnUiThread {
            loadingDialog?.apply {
                if (!isShowing) {
                    return@runOnUiThread
                }
                loading_text.text = message

                if (showCloseButton) {
                    loading_icon.visibility = View.GONE
                    getButton(AlertDialog.BUTTON_POSITIVE).visibility = View.VISIBLE
                }
            }
        }
    }

    override fun setLoadingDialogMessage(errorType: ErrorType?, showCloseButton: Boolean) {
        setLoadingDialogMessage(ErrorMessageConverter.convert(activity!!, errorType!!), showCloseButton)
    }

    override fun dismissLoadingDialog() {
        activity?.runOnUiThread {
            loadingDialog?.apply {
                dismiss()
                loadingDialog = null
            }
        }
    }

    // DeviceDialogListener

    override fun getDeviceConfig(): DeviceConfig {
        return deviceConfigPresenter.deviceConfig
    }

    override fun onDeviceNameErrorMessage(message: DeviceConfigListener.ERROR_MESSAGE) {
        val errorMessage = when (message) {
            DeviceConfigListener.ERROR_MESSAGE.DEVICE_NAME_EMPTY -> getString(R.string.device_dialog_empty_name)
        }
        errorMessage.apply {
            device_name_text_input?.error = errorMessage
        }
    }
}
