/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.GroupList

import android.app.AlertDialog
import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.view.WindowManager
import android.widget.AbsListView
import android.widget.ListView
import android.widget.Toast
import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlab.bluetoothmesh.adk.data_model.group.Group
import com.siliconlabs.bluetoothmesh.App.Activities.Main.MainActivity
import com.siliconlabs.bluetoothmesh.App.Fragments.ControlGroup.ControlGroupFragment
import com.siliconlabs.bluetoothmesh.App.Fragments.Network.NetworkFragment
import com.siliconlabs.bluetoothmesh.App.Fragments.Network.NetworkView
import com.siliconlabs.bluetoothmesh.App.Utils.ErrorMessageConverter
import com.siliconlabs.bluetoothmesh.App.Views.CustomAlertDialogBuilder
import com.siliconlabs.bluetoothmesh.R
import dagger.android.support.DaggerFragment
import kotlinx.android.synthetic.main.dialog_group_add.view.*
import kotlinx.android.synthetic.main.dialog_group_edit.view.*
import kotlinx.android.synthetic.main.dialog_loading.*
import kotlinx.android.synthetic.main.groups_screen.*
import javax.inject.Inject


/**
 * @author Comarch S.A.
 */
class GroupListFragment : DaggerFragment(), GroupListView {
    private val TAG: String = javaClass.canonicalName

    @Inject
    lateinit var groupListPresenter: GroupListPresenter

    private var groupListAdapter: GroupListAdapter? = null

    private var loadingDialog: AlertDialog? = null

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        Log.d(TAG, "onCreateView")
        return inflater.inflate(R.layout.groups_screen, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        Log.d(TAG, "onViewCreated")

        groupListAdapter = GroupListAdapter(context!!, groupListPresenter)
        group_list.adapter = groupListAdapter
        group_list.choiceMode = ListView.CHOICE_MODE_MULTIPLE_MODAL
        group_list.setMultiChoiceModeListener(groupListAdapter)
        group_list.emptyView = ll_empty_view

        group_list.setOnScrollListener(object : AbsListView.OnScrollListener {
            private var lastFirstVisibleItem: Int = 0

            override fun onScroll(view: AbsListView?, firstVisibleItem: Int, visibleItemCount: Int, totalItemCount: Int) {
                if (lastFirstVisibleItem < firstVisibleItem) {
                    fab_add_group.hide()
                } else if (lastFirstVisibleItem > firstVisibleItem) {
                    fab_add_group.show()
                }

                lastFirstVisibleItem = firstVisibleItem
            }

            override fun onScrollStateChanged(view: AbsListView?, scrollState: Int) {
            }
        })

        group_list.setOnItemClickListener { _, _, position, _ ->
            groupListAdapter?.apply {
                onGroupClickListener(getItem(position))
            }
        }

        fab_add_group.setOnClickListener {
            showAddGroupDialog()
        }
    }

    override fun onResume() {
        Log.d(TAG, "onResume")
        super.onResume()
        groupListPresenter.onResume()
    }

    override fun onPause() {
        Log.d(TAG, "onPause")
        super.onPause()
        groupListPresenter.onPause()
        groupListAdapter?.finishActionMode()
    }

    override fun setUserVisibleHint(isVisibleToUser: Boolean) {
        super.setUserVisibleHint(isVisibleToUser)
        if (!isVisibleToUser) {
            groupListAdapter?.finishActionMode()
        }
    }

    //
    override fun refreshList() {
        activity?.runOnUiThread {
            groupListPresenter.refreshList()
            (parentFragment as NetworkFragment).refreshFragment(NetworkView.FRAGMENT.DEVICE_LIST)
        }
    }

    override fun showLoadingDialog() {
        activity?.runOnUiThread {
            val view: View = LayoutInflater.from(activity).inflate(R.layout.dialog_loading, null)
            val builder = CustomAlertDialogBuilder(activity!!, R.style.AppTheme_Light_Dialog_Alert_Wrap)
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

    override fun updateLoadingDialogMessage(loadingMessage: GroupListView.LOADING_DIALOG_MESSAGE, message: String, showCloseButton: Boolean) {
        activity?.runOnUiThread {
            loadingDialog?.apply {
                if (!isShowing) {
                    return@runOnUiThread
                }
                loading_text.apply {
                    text = when (loadingMessage) {
                        GroupListView.LOADING_DIALOG_MESSAGE.REMOVING_GROUP -> context.getString(R.string.group_dialog_loading_text_removing_group).format(message)
                        GroupListView.LOADING_DIALOG_MESSAGE.REMOVING_GROUP_ERROR -> context.getString(R.string.group_dialog_loading_text_removing_group_error).format(message)
                    }
                }
                if (showCloseButton) {
                    loading_icon.visibility = View.GONE
                    getButton(AlertDialog.BUTTON_POSITIVE).visibility = View.VISIBLE
                }
            }
        }
    }

    override fun updateLoadingDialogMessage(loadingMessage: GroupListView.LOADING_DIALOG_MESSAGE, errorType: ErrorType, showCloseButton: Boolean) {
        updateLoadingDialogMessage(loadingMessage, ErrorMessageConverter.convert(activity!!, errorType), showCloseButton)
    }

    override fun dismissLoadingDialog() {
        activity?.runOnUiThread {
            loadingDialog?.apply {
                dismiss()
            }
            loadingDialog = null
        }
    }

    override fun setGroupsList(groupsInfo: Set<Group>) {
        activity?.runOnUiThread {
            groupListAdapter?.clear()
            groupListAdapter?.addAll(groupsInfo.toMutableList())
            groupListAdapter?.notifyDataSetChanged()
        }
    }

    override fun showToast(toastMessage: GroupListView.TOAST_MESSAGE) {
        activity?.runOnUiThread {
            when (toastMessage) {
                GroupListView.TOAST_MESSAGE.ERROR_DELETE_GROUP_EXISTED_NODES -> Toast.makeText(context, getString(R.string.group_delete_error_existed_nodes), Toast.LENGTH_SHORT).show()
                GroupListView.TOAST_MESSAGE.ERROR_CREATING_GROUP -> Toast.makeText(context, getString(R.string.group_create_group_error), Toast.LENGTH_SHORT).show()
            }
        }
    }

    override fun showAddGroupDialog() {
        activity?.runOnUiThread {
            val view: View = LayoutInflater.from(context).inflate(R.layout.dialog_group_add, null)
            val builder = CustomAlertDialogBuilder(context!!, R.style.AppTheme_Light_Dialog_Alert)
            builder.apply {
                setTitle(getString(R.string.group_dialog_add_title))
                setView(view)
                setDismissOnClickPositiveButton(false)
                setPositiveButton(getString(R.string.dialog_positive_add)) { dialog, _ ->
                    val name = view.group_name_add.text.toString()
                    if (groupListPresenter.addGroup(name)) {
                        dialog.dismiss()
                    } else {
                        view.group_name_text_input_add.error = getString(R.string.group_update_error_empty_name_message)
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

    override fun showEditGroupDialog(groupInfo: Group) {
        activity?.runOnUiThread {
            groupListAdapter?.finishActionMode()

            val view: View = LayoutInflater.from(context).inflate(R.layout.dialog_group_edit, null)
            view.apply {
                group_name_edit.setText(groupInfo.name)
                group_name_edit.setSelection(groupInfo.name.length)
                group_key_id.text = groupInfo.appKey.keyIndex.toString()
                network_name.text = groupInfo.subnet.name
            }
            val builder = CustomAlertDialogBuilder(activity!!, R.style.AppTheme_Light_Dialog_Alert)
            builder.apply {
                setTitle(getString(R.string.group_dialog_edit_title))
                setView(view)
                setDismissOnClickPositiveButton(false)
                setPositiveButton(getString(R.string.dialog_positive_save)) { dialog, _ ->
                    val name = view.group_name_edit.text.toString()

                    if (groupListPresenter.updateGroup(groupInfo, name)) {
                        dialog.dismiss()
                    } else {
                        view.group_name_text_input_edit.error = getString(R.string.group_update_error_empty_name_message)
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

    override fun showDeleteGroupDialog(groupInfo: List<Group>) {
        activity?.runOnUiThread {
            val builder = AlertDialog.Builder(activity, R.style.AppTheme_Light_Dialog_Alert)
            builder.apply {
                setTitle(getString(R.string.group_dialog_delete_title))
                setPositiveButton(getString(R.string.dialog_positive_delete)) { dialog, _ ->
                    groupListPresenter.deleteGroups(groupInfo)
                    groupListAdapter?.finishActionMode()
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

    override fun showDeleteGroupLocallyDialog(errorType: ErrorType, group: Group) {
        activity?.runOnUiThread {
            AlertDialog.Builder(activity, R.style.AppTheme_Light_Dialog_Alert).apply {
                setTitle(R.string.group_dialog_delete_locally_title)

                setMessage(getString(R.string.group_dialog_delete_locally_message, ErrorMessageConverter.convert(activity!!, errorType), group.name))

                setPositiveButton(context.getString(R.string.dialog_positive_delete)) { dialog, _ ->
                    groupListPresenter.deleteGroupLocally(group)
                    dialog.dismiss()
                }

                setNegativeButton(R.string.dialog_negative_cancel) { dialog, _ ->
                    dialog.dismiss()
                }

                create().show()
            }
        }
    }

    override fun onGroupClickListener(groupInfo: Group) {
        groupListPresenter.onGroupClicked(groupInfo)

        val fragment = ControlGroupFragment()
        val mainActivity = activity as MainActivity
        mainActivity.showFragment(fragment, true, true)
    }
}
