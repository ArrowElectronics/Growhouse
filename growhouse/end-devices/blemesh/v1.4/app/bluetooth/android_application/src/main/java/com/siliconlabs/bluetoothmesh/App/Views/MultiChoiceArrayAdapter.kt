/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Views

import android.content.Context
import android.util.SparseBooleanArray
import android.view.ActionMode
import android.view.Menu
import android.view.MenuItem
import android.widget.AbsListView
import android.widget.ArrayAdapter
import com.siliconlabs.bluetoothmesh.R

/**
 * @author Comarch S.A.
 */
abstract class MultiChoiceArrayAdapter<T>(ctx: Context, resourceId: Int) : ArrayAdapter<T>(ctx, resourceId), AbsListView.MultiChoiceModeListener {

    val selectedItemsIds: SparseBooleanArray = SparseBooleanArray()
    var multipleMode = false

    private var mode: ActionMode? = null

    abstract fun onDeleteClickListener(item: List<T>)

    override fun onActionItemClicked(mode: ActionMode?, item: MenuItem?): Boolean {
        this.mode = mode

        return when (item?.itemId) {
            R.id.delete -> {
                val selectedIds = getSelectedIds()
                val selectedItems: ArrayList<T> = ArrayList()

                for (i in selectedIds.size() downTo 0) {
                    if (selectedIds.valueAt(i)) {
                        val index = selectedIds.keyAt(i)
                        if (index < count) {
                            selectedItems.add(getItem(index))
                        }
                    }
                }

                onDeleteClickListener(selectedItems)
                true
            }
            else -> false
        }
    }

    override fun onItemCheckedStateChanged(mode: ActionMode?, position: Int, id: Long, checked: Boolean) {
        this.mode = mode

        toggleSelection(position, checked)

        val checkedCount = selectedItemsIds.size()
        mode?.title = context.getString(R.string.multi_choice_adapter_title).format(checkedCount)
    }

    override fun onCreateActionMode(mode: ActionMode?, menu: Menu?): Boolean {
        this.mode = mode

        mode?.menuInflater?.inflate(R.menu.menu_edit_listview, menu)
        setEditMode(true)
        return true
    }

    override fun onPrepareActionMode(mode: ActionMode?, menu: Menu?): Boolean {
        this.mode = mode

        return false
    }

    override fun onDestroyActionMode(mode: ActionMode?) {
        this.mode = mode

        removeSelection()
        setEditMode(false)
    }

    fun finishActionMode(){
        mode?.finish()
    }

    private fun setEditMode(enabled: Boolean) {
        multipleMode = enabled
    }

    private fun toggleSelection(position: Int, checked: Boolean) {
        if (checked) {
            selectedItemsIds.put(position, checked)
        } else {
            selectedItemsIds.delete(position)
        }

        notifyDataSetChanged()
    }

    private fun getSelectedIds(): SparseBooleanArray {
        return selectedItemsIds
    }

    private fun removeSelection() {
        selectedItemsIds.clear()
        notifyDataSetChanged()
    }
}