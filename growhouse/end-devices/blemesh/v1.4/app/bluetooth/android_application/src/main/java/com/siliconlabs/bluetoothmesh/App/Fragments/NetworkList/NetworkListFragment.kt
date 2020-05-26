/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.NetworkList

import android.app.AlertDialog
import android.content.Intent
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.view.WindowManager
import android.widget.AbsListView
import android.widget.ListView
import android.widget.Toast
import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlab.bluetoothmesh.adk.data_model.subnet.Subnet
import com.siliconlabs.bluetoothmesh.App.Activities.Main.MainActivity
import com.siliconlabs.bluetoothmesh.App.Fragments.Network.NetworkFragment
import com.siliconlabs.bluetoothmesh.App.Utils.Converters
import com.siliconlabs.bluetoothmesh.App.Utils.ErrorMessageConverter
import com.siliconlabs.bluetoothmesh.App.Views.CustomAlertDialogBuilder
import com.siliconlabs.bluetoothmesh.R
import dagger.android.support.DaggerFragment
import kotlinx.android.synthetic.main.dialog_loading.*
import kotlinx.android.synthetic.main.dialog_network_add.view.*
import kotlinx.android.synthetic.main.dialog_network_edit.view.*
import kotlinx.android.synthetic.main.network_screen.*
import javax.inject.Inject

/**
 * @author Comarch S.A.
 */
class NetworkListFragment : DaggerFragment(), NetworkListAdapter.NetworkItemListener, NetworkListView {
    private val TAG: String = javaClass.canonicalName

    @Inject
    lateinit var networkListPresenter: NetworkListPresenter

    private var networkListAdapter: NetworkListAdapter? = null

    private var loadingDialog: AlertDialog? = null

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        return inflater.inflate(R.layout.network_screen, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        networkListAdapter = NetworkListAdapter(context!!, this)
        network_list.adapter = networkListAdapter
        network_list.choiceMode = ListView.CHOICE_MODE_MULTIPLE_MODAL
        network_list.setMultiChoiceModeListener(networkListAdapter)
        network_list.emptyView = ll_empty_view

        network_list.setOnItemClickListener { _, _, position, _ ->
            networkListAdapter?.apply {
                onNetworkClickListener(getItem(position))
            }
        }

        network_list.setOnScrollListener(object : AbsListView.OnScrollListener {
            private var lastFirstVisibleItem: Int = 0

            override fun onScroll(view: AbsListView?, firstVisibleItem: Int, visibleItemCount: Int, totalItemCount: Int) {
                if (lastFirstVisibleItem < firstVisibleItem) {
                    fab_add_network.hide()
                } else if (lastFirstVisibleItem > firstVisibleItem) {
                    fab_add_network.show()
                }

                lastFirstVisibleItem = firstVisibleItem
            }

            override fun onScrollStateChanged(view: AbsListView?, scrollState: Int) {
            }
        })

        fab_add_network.setOnClickListener {
            showAddNetworkDialog()
        }
    }

    override fun onResume() {
        super.onResume()
        networkListPresenter.onResume()
    }

    override fun onPause() {
        super.onPause()
        networkListPresenter.onPause()
        networkListAdapter?.finishActionMode()
    }

    override fun setUserVisibleHint(isVisibleToUser: Boolean) {
        super.setUserVisibleHint(isVisibleToUser)
        if (!isVisibleToUser) {
            networkListAdapter?.finishActionMode()
        }
    }

    // Dialogs

    override fun showDeleteNetworkDialog(networkInfo: List<Subnet>) {
        activity?.runOnUiThread {
            val builder = AlertDialog.Builder(context, R.style.AppTheme_Light_Dialog_Alert)
            builder.apply {
                setTitle(getString(R.string.network_dialog_delete_title))
                setPositiveButton(getString(R.string.dialog_positive_delete)) { dialog, _ ->
                    networkListPresenter.deleteNetwork(networkInfo)
                    networkListAdapter?.finishActionMode()
                    dialog.dismiss()
                }
                setNegativeButton(R.string.dialog_negative_cancel) { dialog, _ ->
                    dialog.dismiss()
                }
            }

            val dialog = builder.create()
            dialog.apply {
                show()
            }
        }
    }

    override fun showDeleteNetworkLocallyDialog(subnet: Subnet, errorType: ErrorType) {
        activity?.runOnUiThread {
            AlertDialog.Builder(context, R.style.AppTheme_Light_Dialog_Alert).apply {
                setTitle(R.string.network_dialog_delete_locally_title)
                setMessage(getString(R.string.network_dialog_delete_locally_message, ErrorMessageConverter.convert(activity!!, errorType), subnet.name))
                setPositiveButton(R.string.dialog_positive_delete) { dialog, _ ->
                    networkListPresenter.deleteNetworkLocally(subnet)
                    dialog.dismiss()
                }
                setNegativeButton(R.string.dialog_negative_cancel) { dialog, _ ->
                    dialog.dismiss()
                }
                create().show()
            }
        }
    }

    override fun showEditNetworkDialog(networkInfo: Subnet) {
        activity?.runOnUiThread {
            networkListAdapter?.finishActionMode()

            val view: View = LayoutInflater.from(context).inflate(R.layout.dialog_network_edit, null)
            view.apply {
                network_name_edit.setText(networkInfo.name)
                network_name_edit.setSelection(network_name_edit.text.length)
                network_key_id.text = networkInfo.netKey.keyIndex.toString()
                network_key.text = Converters.getHexValue(networkInfo.netKey.key)

                network_export.setOnClickListener {
                    networkListPresenter.exportNetwork(networkInfo)
                }
            }
            val builder = CustomAlertDialogBuilder(activity!!, R.style.AppTheme_Light_Dialog_Alert)
            builder.apply {
                setTitle(getString(R.string.network_dialog_edit_title))
                setView(view)
                setDismissOnClickPositiveButton(false)
                setPositiveButton(getString(R.string.dialog_positive_save)) { dialog, _ ->
                    val newName: String = view.network_name_edit.text.toString()
                    if (networkListPresenter.updateNetwork(networkInfo, newName)) {
                        dialog.dismiss()
                    } else {
                        view.network_name_text_input_edit.error = getString(R.string.network_update_error_empty_name_message)
                    }
                }
                setNegativeButton(R.string.dialog_negative_cancel) { dialog, _ ->
                }
            }

            val dialog = builder.create()
            dialog.apply {
                window.setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE)
                show()
            }
        }
    }

    override fun showAddNetworkDialog() {
        activity?.runOnUiThread {
            val view: View = LayoutInflater.from(context).inflate(R.layout.dialog_network_add, null)
            val builder = CustomAlertDialogBuilder(activity!!, R.style.AppTheme_Light_Dialog_Alert)
            builder.apply {
                setTitle(getString(R.string.network_dialog_add_title))
                setView(view)
                setDismissOnClickPositiveButton(false)
                setPositiveButton(getString(R.string.dialog_positive_add)) { dialog, _ ->
                    if (networkListPresenter.addNetwork(view.network_name_add.text.toString())) {
                        dialog.dismiss()
                    } else {
                        view.network_name_text_input_add.error = getString(R.string.network_update_error_empty_name_message)
                    }
                }
                setNegativeButton(getString(R.string.dialog_negative_cancel)) { dialog, _ ->
                }
            }

            val dialog = builder.create()
            dialog.apply {
                window.setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE)
                show()
            }
        }
    }

    // View

    override fun showToast(toastMessage: NetworkListView.TOAST_MESSAGE) {
        activity?.runOnUiThread {
            when (toastMessage) {
                NetworkListView.TOAST_MESSAGE.SUCCESS_ADD -> Toast.makeText(context, getString(R.string.network_add_successful_message), Toast.LENGTH_SHORT).show()
                NetworkListView.TOAST_MESSAGE.SUCCESS_UPDATE -> Toast.makeText(context, getString(R.string.network_update_successful_message), Toast.LENGTH_SHORT).show()
                NetworkListView.TOAST_MESSAGE.SUCCESS_DELETE -> Toast.makeText(context, getString(R.string.network_update_successful_message), Toast.LENGTH_SHORT).show()
                NetworkListView.TOAST_MESSAGE.ERROR_CREATING_NETWORK -> Toast.makeText(context, getString(R.string.network_create_error), Toast.LENGTH_SHORT).show()
            }
        }
    }

    override fun setNetworkList(networkList: Set<Subnet>) {
        activity?.runOnUiThread {
            networkListAdapter?.clear()
            networkListAdapter?.addAll(networkList.toMutableList())
            networkListAdapter?.notifyDataSetChanged()
        }
    }

    override fun showLoadingDialog() {
        activity?.runOnUiThread {
            val view: View = LayoutInflater.from(activity).inflate(R.layout.dialog_loading, null)
            val builder = CustomAlertDialogBuilder(context!!, R.style.AppTheme_Light_Dialog_Alert_Wrap)
            builder.apply {
                setView(view)
                setCancelable(false)
                setPositiveButton(activity!!.getString(R.string.dialog_positive_ok)) { dialog, _ ->
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

    override fun updateLoadingDialogMessage(loadingMessage: NetworkListView.LOADING_DIALOG_MESSAGE, message: String, showCloseButton: Boolean) {
        activity?.runOnUiThread {
            loadingDialog?.apply {
                if (!isShowing) {
                    return@runOnUiThread
                }
                loading_text.apply {
                    text = when (loadingMessage) {
                        NetworkListView.LOADING_DIALOG_MESSAGE.REMOVING_NETWORK -> context.getString(R.string.network_dialog_loading_text_removing_network).format(message)
                        NetworkListView.LOADING_DIALOG_MESSAGE.REMOVING_NETWORK_ERROR -> context.getString(R.string.network_dialog_loading_text_removing_network_error).format(message)
                        NetworkListView.LOADING_DIALOG_MESSAGE.CONNECTING_TO_NETWORK -> context.getString(R.string.network_dialog_loading_text_connecting_to_network).format(message)
                        NetworkListView.LOADING_DIALOG_MESSAGE.CONNECTING_TO_NETWORK_ERROR -> context.getString(R.string.network_dialog_loading_text_connecting_to_network_error).format(message)
                    }
                }
                if (showCloseButton) {
                    loading_icon.visibility = View.GONE
                    getButton(AlertDialog.BUTTON_POSITIVE).visibility = View.VISIBLE
                }
            }
        }
    }

    override fun updateLoadingDialogMessage(loadingMessage: NetworkListView.LOADING_DIALOG_MESSAGE, errorType: ErrorType, showCloseButton: Boolean) {
        updateLoadingDialogMessage(loadingMessage, ErrorMessageConverter.convert(activity!!, errorType), showCloseButton)
    }

    override fun dismissLoadingDialog() {
        activity?.runOnUiThread {
            loadingDialog?.apply {
                dismiss()
            }
        }
    }

    override fun onDeleteClickListener(networkInfo: List<Subnet>) {
        showDeleteNetworkDialog(networkInfo)
    }

    override fun onEditClickListener(networkInfo: Subnet) {
        showEditNetworkDialog(networkInfo)
    }

    override fun onNetworkClickListener(networkInfo: Subnet) {
        networkListPresenter.networkClicked(networkInfo)
    }

    override fun showNetworkFragment() {
        val fragment = NetworkFragment()
        val mainActivity = activity as MainActivity
        mainActivity.showFragment(fragment, true, true)
    }

    override fun showIntent(intent: Intent) {
        activity?.startActivity(intent)
    }
}