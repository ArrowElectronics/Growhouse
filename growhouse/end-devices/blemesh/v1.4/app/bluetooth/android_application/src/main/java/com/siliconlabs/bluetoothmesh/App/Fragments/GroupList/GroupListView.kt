/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.GroupList

import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlab.bluetoothmesh.adk.data_model.group.Group

/**
 * @author Comarch S.A.
 */
interface GroupListView {

    fun setGroupsList(groupsInfo: Set<Group>)

    fun showToast(toastMessage: TOAST_MESSAGE)

    fun showAddGroupDialog()

    fun showEditGroupDialog(groupInfo: Group)

    fun showDeleteGroupDialog(groupInfo: List<Group>)

    fun showDeleteGroupLocallyDialog(errorType: ErrorType, group: Group)

    fun onGroupClickListener(groupInfo: Group)

    fun showLoadingDialog()

    fun updateLoadingDialogMessage(loadingMessage: LOADING_DIALOG_MESSAGE, message: String = "", showCloseButton: Boolean = false)

    fun updateLoadingDialogMessage(loadingMessage: LOADING_DIALOG_MESSAGE, errorType: ErrorType, showCloseButton: Boolean = false)

    fun dismissLoadingDialog()

    fun refreshList()

    enum class LOADING_DIALOG_MESSAGE {
        REMOVING_GROUP,
        REMOVING_GROUP_ERROR
    }

    enum class TOAST_MESSAGE {
        ERROR_DELETE_GROUP_EXISTED_NODES,
        ERROR_CREATING_GROUP
    }

}