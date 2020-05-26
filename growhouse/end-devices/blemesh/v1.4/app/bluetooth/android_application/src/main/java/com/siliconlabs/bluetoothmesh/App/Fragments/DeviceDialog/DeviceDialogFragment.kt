/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.DeviceDialog

import android.app.AlertDialog
import android.app.Dialog
import android.content.Context
import android.content.DialogInterface
import android.os.Bundle
import android.support.v4.app.DialogFragment
import android.support.v4.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.view.WindowManager
import com.siliconlabs.bluetoothmesh.App.Dialogs.DeviceEditionDialogsListener
import com.siliconlabs.bluetoothmesh.App.Models.DeviceConfig
import com.siliconlabs.bluetoothmesh.App.Views.CustomAlertDialogBuilder
import com.siliconlabs.bluetoothmesh.R
import dagger.android.AndroidInjector
import dagger.android.DispatchingAndroidInjector
import dagger.android.support.AndroidSupportInjection
import dagger.android.support.HasSupportFragmentInjector
import kotlinx.android.synthetic.main.dialog_device.view.*
import javax.inject.Inject

/**
 * @author Comarch S.A.
 */
class DeviceDialogFragment : DialogFragment(), DeviceDialogView, HasSupportFragmentInjector {

    @Inject
    lateinit var childFragmentInjector: DispatchingAndroidInjector<Fragment>

    override fun supportFragmentInjector(): AndroidInjector<Fragment> {
        return childFragmentInjector
    }

    @Inject
    lateinit var deviceDialogPresenter: DeviceDialogPresenter

    var deviceEditionDialogsListener: DeviceEditionDialogsListener? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setStyle(DialogFragment.STYLE_NORMAL, R.style.AppTheme_Light_Dialog_Alert)
    }

    override fun onAttach(context: Context) {
        AndroidSupportInjection.inject(this)
        super.onAttach(context)
    }

    override fun onDismiss(dialog: DialogInterface?) {
        deviceDialogPresenter.onCancelDeviceConfigClicked();
        super.onDismiss(dialog)
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        val view = inflater.inflate(R.layout.dialog_device, container)
        view?.apply {
            view_pager.adapter = DeviceDialogPageAdapter(childFragmentManager, context)

            tab_layout.setupWithViewPager(view_pager)
            buttonPositive.setOnClickListener {
                if (deviceDialogPresenter.onSaveDeviceConfigClicked()) {
                    dialog.dismiss()
                }
            }
            buttonNegative.setOnClickListener {
                deviceDialogPresenter.onCancelDeviceConfigClicked()
                dialog.dismiss()
            }
        }
        return view
    }

    override fun onResume() {
        super.onResume()
        dialog?.window?.clearFlags(WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE or WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM)
        dialog?.window?.setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_VISIBLE)
        deviceDialogPresenter.onResume()
        deviceDialogPresenter.deviceEditionDialogsListener = deviceEditionDialogsListener
    }

    override fun onPause() {
        super.onPause()
        deviceDialogPresenter.onPause()
        deviceDialogPresenter.deviceEditionDialogsListener = null
    }

    override fun showDisableProxyAttentionDialog(deviceConfig: DeviceConfig, clickListener: DialogInterface.OnClickListener) {
        val builder = CustomAlertDialogBuilder(activity!!, R.style.AppTheme_Light_Dialog_Alert)
        builder.apply {
            setTitle(getString(R.string.device_config_proxy_disable_attention_title))
            setMessage(getString(R.string.device_config_proxy_disable_attention_message))
            setPositiveButton(getString(R.string.dialog_positive_ok)) { dialog, _ ->
                clickListener.onClick(dialog, AlertDialog.BUTTON_POSITIVE)
                this@DeviceDialogFragment.dismiss()
            }
            setNegativeButton(getString(R.string.dialog_negative_cancel)) { dialog, _ ->
                clickListener.onClick(dialog, AlertDialog.BUTTON_NEGATIVE)
            }
        }

        val dialog = builder.create()
        dialog.apply {
            show()
        }
    }
}