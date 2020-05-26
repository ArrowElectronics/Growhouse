/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.NetworkList

import android.content.Context
import android.support.v7.widget.RecyclerView
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.siliconlab.bluetoothmesh.adk.data_model.subnet.Subnet
import com.siliconlabs.bluetoothmesh.App.Views.MultiChoiceArrayAdapter
import com.siliconlabs.bluetoothmesh.R
import kotlinx.android.synthetic.main.networks_adapter.view.*
import java.util.*


/**
 * @author Comarch S.A.
 */
class NetworkListAdapter(ctx: Context, private val networkItemListener: NetworkItemListener) :
        MultiChoiceArrayAdapter<Subnet>(ctx, 0) {

    override fun getView(position: Int, convertView: View?, parent: ViewGroup?): View {
        val networkInfo = getItem(position)
        var holder: ViewHolder
        var view: View

        if (convertView == null) {
            view = LayoutInflater.from(context).inflate(R.layout.networks_adapter, parent, false)
            holder = ViewHolder(view)
            view.tag = holder
        } else {
            view = convertView
            holder = convertView.tag as ViewHolder
        }

        holder.itemView.apply {
            tv_group_name.text = networkInfo.name
            tv_group_devices.text = context.getString(R.string.network_adapter_devices_label).format(networkInfo.nodes.size)
            tv_network_groups.text = context.getString(R.string.network_adapter_groups_label).format(networkInfo.groups.size)
            tv_network_key_index.text = networkInfo.netKey.keyIndex.toString()

            cb_selected.isChecked = selectedItemsIds.indexOfKey(position) >= 0

            if (multipleMode) {
                cb_selected.visibility = View.VISIBLE
                iv_config.visibility = View.VISIBLE
            } else {
                cb_selected.visibility = View.GONE
                iv_config.visibility = View.GONE
            }

            iv_config.setOnClickListener {
                networkItemListener.onEditClickListener(networkInfo)
            }
        }

        return view
    }

    override fun addAll(collection: MutableCollection<out Subnet>?) {
        super.addAll(collection)
        sort(NetworkInfoComparator())
    }

    override fun onDeleteClickListener(item: List<Subnet>) {
        networkItemListener.onDeleteClickListener(item)
    }

    interface NetworkItemListener {
        fun onDeleteClickListener(networkInfo: List<Subnet>)
        fun onEditClickListener(networkInfo: Subnet)
        fun onNetworkClickListener(networkInfo: Subnet)
    }

    private class ViewHolder(view: View) : RecyclerView.ViewHolder(view)

    // Comparator

    class NetworkInfoComparator : Comparator<Subnet> {
        override fun compare(o1: Subnet, o2: Subnet): Int {
            return o1.netKey.keyIndex.compareTo(o2.netKey.keyIndex)
        }
    }
}