/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Views

import android.app.AlertDialog
import android.content.Context
import android.content.DialogInterface

/**
 * @author Comarch S.A.
 */
class CustomAlertDialogBuilder : AlertDialog.Builder {

    private var dismissOnClickPositiveButton: Boolean = true

    private var positiveButtonListener: DialogInterface.OnClickListener? = null

    constructor(context: Context) : super(context)
    constructor(context: Context, themeResId: Int) : super(context, themeResId)

    fun setDismissOnClickPositiveButton(cancelable: Boolean) {
        this.dismissOnClickPositiveButton = cancelable
    }

    override fun setPositiveButton(textId: Int, listener: DialogInterface.OnClickListener?): AlertDialog.Builder {
        positiveButtonListener = listener
        return super.setPositiveButton(textId, listener)
    }

    override fun setPositiveButton(text: CharSequence?, listener: DialogInterface.OnClickListener?): AlertDialog.Builder {
        positiveButtonListener = listener
        return super.setPositiveButton(text, listener)
    }

    override fun create(): AlertDialog {
        val alertDialog: AlertDialog = super.create()
        if (!dismissOnClickPositiveButton) {
            alertDialog.apply {
                setOnShowListener { dialog ->
                    val positiveButton = alertDialog.getButton(AlertDialog.BUTTON_POSITIVE)
                    positiveButton.setOnClickListener {
                        positiveButtonListener?.onClick(dialog, AlertDialog.BUTTON_POSITIVE)
                    }
                }
            }
        }

        return alertDialog
    }
}