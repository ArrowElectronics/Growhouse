/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.GroupList

import android.util.Log
import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlab.bluetoothmesh.adk.data_model.group.Group
import com.siliconlab.bluetoothmesh.adk.data_model.group.GroupChangeNameException
import com.siliconlab.bluetoothmesh.adk.data_model.subnet.GroupCreationException
import com.siliconlab.bluetoothmesh.adk.data_model.subnet.Subnet
import com.siliconlabs.bluetoothmesh.App.BasePresenter
import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic
import com.siliconlabs.bluetoothmesh.App.Models.MeshNetworkManager
import java.util.concurrent.Executors

/**
 * @author Comarch S.A.
 */
class GroupListPresenter(private val groupListView: GroupListView, private val meshLogic: MeshLogic, private val meshNetworkManager: MeshNetworkManager) : BasePresenter, GroupListAdapter.GroupItemListener {
    private val TAG: String = javaClass.canonicalName

    private val subnet: Subnet = meshLogic.currentSubnet!!

    private val taskExecutor = Executors.newSingleThreadScheduledExecutor()
    private val taskList = mutableListOf<Runnable>()

    override fun onResume() {
        groupListView.refreshList()
    }

    override fun onPause() {
    }

    fun refreshList() {
        groupListView.setGroupsList(subnet.groups)
    }

    fun startTasks() {
        if (taskList.isNotEmpty()) {
            groupListView.showLoadingDialog()

            takeNextTask()
        }
    }

    fun takeNextTask() {
        if (taskList.isNotEmpty()) {
            val task = taskList.first()
            taskList.remove(task)
            taskExecutor.execute(task)
        } else {
            groupListView.dismissLoadingDialog()
            refreshList()
        }
    }

    // View callbacks

    fun addGroup(name: String): Boolean {
        if (name.trim().isEmpty()) {
            return false
        }

        try {
            meshNetworkManager.createGroup(name, subnet)
        } catch (e: GroupCreationException) {
            Log.e(TAG, e.toString())
            groupListView.showToast(GroupListView.TOAST_MESSAGE.ERROR_CREATING_GROUP)
        }

        refreshList()
        return true
    }

    fun deleteGroups(groupsInfo: List<Group>) {
        groupsInfo.forEach {
            taskList.add(Runnable {
                groupListView.updateLoadingDialogMessage(GroupListView.LOADING_DIALOG_MESSAGE.REMOVING_GROUP, it.name)
                meshNetworkManager.removeGroup(it, object : MeshNetworkManager.DeleteGroupsCallback {
                    override fun success() {
                        takeNextTask()
                    }

                    override fun error(group: Group?, errorType: ErrorType?) {
                        groupListView.dismissLoadingDialog()
                        groupListView.showDeleteGroupLocallyDialog(errorType!!, group!!)
                        taskList.clear()
                        refreshList()
                    }
                })
            })
        }

        startTasks()
    }

    fun deleteGroupLocally(group: Group) {
        group.removeOnlyFromLocalStructure()
        refreshList()
    }

    fun updateGroup(groupInfo: Group, newName: String): Boolean {
        if (newName.trim().isEmpty()) {
            return false
        }

        try {
            groupInfo.name = newName
        } catch (e: GroupChangeNameException) {
            return false
        }

        refreshList()
        return true
    }

    fun onGroupClicked(group: Group) {
        meshLogic.currentGroup = group
    }

    // GroupListAdapter.GroupItemListener

    override fun onDeleteClickListener(groupInfo: List<Group>) {
        groupListView.showDeleteGroupDialog(groupInfo)
    }

    override fun onEditClickListener(groupInfo: Group) {
        groupListView.showEditGroupDialog(groupInfo)
    }

    override fun onGroupClickListener(groupInfo: Group) {
        groupListView.onGroupClickListener(groupInfo)
    }

}
