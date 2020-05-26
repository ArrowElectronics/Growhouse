/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.GroupList

import android.content.Context
import android.support.v7.widget.RecyclerView
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.siliconlab.bluetoothmesh.adk.data_model.group.Group
import com.siliconlabs.bluetoothmesh.App.Views.MultiChoiceArrayAdapter
import com.siliconlabs.bluetoothmesh.R
import kotlinx.android.synthetic.main.groups_adapter.view.*
import java.util.*

/**
 * @author Comarch S.A.
 */
class GroupListAdapter(ctx: Context, private val groupItemListener: GroupItemListener) :
        MultiChoiceArrayAdapter<Group>(ctx, 0) {

    override fun getView(position: Int, convertView: View?, parent: ViewGroup?): View {
        val groupInfo = getItem(position)
        var holder: ViewHolder
        var view: View

        if (convertView == null) {
            view = LayoutInflater.from(context).inflate(R.layout.groups_adapter, parent, false)
            holder = ViewHolder(view)
            view.tag = holder
        } else {
            view = convertView
            holder = convertView.tag as ViewHolder
        }

        holder.itemView.apply {
            tv_group_name.text = groupInfo.name
            tv_group_devices.text = context.getString(R.string.group_adapter_devices_label).format(groupInfo.nodes.size)
            tv_group_key_index.text = groupInfo.appKey.keyIndex.toString()

            cb_selected.isChecked = selectedItemsIds.indexOfKey(position) >= 0

            if (multipleMode) {
                cb_selected.visibility = View.VISIBLE
                iv_config.visibility = View.VISIBLE
            } else {
                cb_selected.visibility = View.GONE
                iv_config.visibility = View.GONE
            }

            iv_config.setOnClickListener {
                groupItemListener.onEditClickListener(groupInfo)
            }
        }

        return view
    }

    override fun addAll(collection: MutableCollection<out Group>?) {
        super.addAll(collection)
        sort(GroupInfoComparator())
    }

    override fun onDeleteClickListener(item: List<Group>) {
        groupItemListener.onDeleteClickListener(item)
    }

    interface GroupItemListener {
        fun onDeleteClickListener(groupInfo: List<Group>)
        fun onEditClickListener(groupInfo: Group)
        fun onGroupClickListener(groupInfo: Group)
    }

    private class ViewHolder(view: View) : RecyclerView.ViewHolder(view)

    // Comparator

    class GroupInfoComparator : Comparator<Group> {
        override fun compare(o1: Group, o2: Group): Int {
            return o1.appKey.keyIndex.compareTo(o2.appKey.keyIndex)
        }
    }
}