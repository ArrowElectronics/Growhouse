/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Dialogs

import android.app.Activity
import android.app.Dialog
import android.content.Context
import android.text.InputType
import android.view.View
import android.view.Window
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import android.widget.Toast
import com.siliconlabs.bluetoothmesh.App.Fragments.Scanner.ScannerPresenter
import com.siliconlabs.bluetoothmesh.R
import java.nio.ByteBuffer

class AuthDialogs {
    companion object {
        fun oob_display_UI(context : Context, uuid: ByteArray, input_action: Int, input_size: Int, auth_data: ByteArray) {
            val authDisplayDialog = Dialog(context)
            authDisplayDialog.requestWindowFeature(Window.FEATURE_NO_TITLE)
            authDisplayDialog.setContentView(R.layout.auth_display_dialog)
            authDisplayDialog.setCanceledOnTouchOutside(false)
            val auth = authDisplayDialog.findViewById<View>(R.id.auth) as TextView
            val ok = authDisplayDialog.findViewById<View>(R.id.ok) as Button
            val cancel = authDisplayDialog.findViewById<View>(R.id.cancel_btn) as Button
            when (input_action) {
                0, 1, 2 -> {
                    val code = ByteArray(4)
                    var j = 3
                    for (i in auth_data.size downTo auth_data.size - 4 + 1) {
                        code[j] = auth_data[i - 1]
                        j--
                    }

                    val show = ByteBuffer.wrap(code).int
                    auth.text = "" + show
                }
                3 -> {
                    val data = ByteArray(input_size)
                    for (i in 0 until input_size) {
                        data[i] = auth_data[i]
                    }
                    val s = String(data)
                    auth.text = s
                }
                else -> {
                }
            }
            authDisplayDialog.show()
            ok.setOnClickListener { authDisplayDialog.dismiss()}
            cancel.setOnClickListener { authDisplayDialog.dismiss()}
        }


        fun oob_request_UI(activity: Activity, uuid: ByteArray, output_action: Int, output_size: Int, presenter: ScannerPresenter) {
            val authRequestDialog = Dialog(activity)
            authRequestDialog.requestWindowFeature(Window.FEATURE_NO_TITLE)
            authRequestDialog.setContentView(R.layout.auth_request_dialog)
            authRequestDialog.setCanceledOnTouchOutside(false)
            val auth = authRequestDialog.findViewById<View>(R.id.auth) as EditText
            val ok = authRequestDialog.findViewById<View>(R.id.ok) as Button
            val cancel = authRequestDialog.findViewById<View>(R.id.cancel_btn) as Button
            when (output_action) {
                0, 1, 2, 3 -> {
                    auth.inputType = InputType.TYPE_CLASS_NUMBER
                    ok.setOnClickListener {
                        val secret = auth.text.toString()
                        val answer_int = Integer.parseInt(secret).toLong()
                        val answer = ByteArray(output_size)
                        val answer_array = ByteArray(16)
                        for (i in 0 until output_size) {
                            answer[output_size - 1 - i] = answer_int.ushr(i * 8).toByte()
                        }
                        for (i in answer_array.indices) {
                            val output_offset = answer_array.size - output_size
                            if (i >= output_offset) {
                                answer_array[i] = answer[i - output_offset]
                            } else
                                answer_array[i] = 0x00.toByte()
                        }
//                        if (presenter.meshLogic.bluetoothMesh.oobAuthKey(uuid, answer_array) == 0) {
//                            activity.runOnUiThread(Runnable { authRequestDialog.dismiss() })
//                        } else {
//                            activity.runOnUiThread(Runnable { Toast.makeText(activity, "Authentication Error", Toast.LENGTH_SHORT).show() })
//                            authRequestDialog.dismiss()
//                        }
                    }
                }
                4 -> ok.setOnClickListener {
                    if (auth.text != null) {
                        val secret = auth.text.toString()
                        val answer_array = ByteArray(16)
                        val answer = secret.toByteArray()
                        for (i in answer_array.indices) {
                            if (i < answer.size) {
                                answer_array[i] = answer[i]
                            } else
                                answer_array[i] = 0x00.toByte()
                        }
//                        if (presenter.meshLogic.bluetoothMesh.oobAuthKey(uuid, answer_array) == 0) {
//                            activity.runOnUiThread(Runnable { authRequestDialog.dismiss() })
//                        } else {
//                            activity.runOnUiThread(Runnable { Toast.makeText(activity, "Authentication Error", Toast.LENGTH_SHORT).show() })
//                            authRequestDialog.dismiss()
//                        }
                    }
                }
                else -> {
                }
            }
            authRequestDialog.show()
            cancel.setOnClickListener {
                authRequestDialog.dismiss()
            }
        }
    }
}