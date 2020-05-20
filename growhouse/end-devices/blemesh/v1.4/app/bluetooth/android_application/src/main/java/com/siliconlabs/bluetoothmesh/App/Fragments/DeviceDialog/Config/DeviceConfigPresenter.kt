/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.DeviceDialog.Config

import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlab.bluetoothmesh.adk.configuration_control.CheckNodeBehaviourCallback
import com.siliconlab.bluetoothmesh.adk.configuration_control.ConfigurationControl
import com.siliconlab.bluetoothmesh.adk.data_model.node.Node
import com.siliconlabs.bluetoothmesh.App.BasePresenter
import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic
import com.siliconlabs.bluetoothmesh.App.Models.DeviceConfig
import java.util.concurrent.Executors

/**
 * @author Comarch S.A.
 */
class DeviceConfigPresenter(private val deviceConfigView: DeviceConfigView, private val meshLogic: MeshLogic) : BasePresenter {
    private val TAG: String = javaClass.canonicalName

    private val meshNode = meshLogic.deviceToConfigure!!
    private val configurationControl: ConfigurationControl = ConfigurationControl(meshNode.node)

    var deviceConfig: DeviceConfig = DeviceConfig()

    private val taskExecutor = Executors.newSingleThreadScheduledExecutor()
    private val taskList = mutableListOf<Runnable>()

    //data
    private var isProxy = false
    private var isRelay = false
    private var isFriend = false
    private var supportFriend = false
    private var supportLowPower = false

    override fun onResume() {
        getDeviceConfig()
    }

    override fun onPause() {
    }

    private fun getDeviceConfig() {
        meshNode.node.deviceCompositionData?.apply {
            supportFriend = supportsFriend()
            supportLowPower = supportsLowPower()
        }

        taskList.add(getProxy())
        taskList.add(getRelay())
        if (supportFriend) {
            taskList.add(getFriend())
        }

        startTasks()
    }


    fun startTasks() {
        if (taskList.isNotEmpty()) {
            deviceConfigView.showLoadingDialog()

            takeNextTask()
        }
    }

    fun takeNextTask() {
        if (taskList.isNotEmpty()) {
            val task = taskList.first()
            taskList.remove(task)
            taskExecutor.execute(task)
        } else {
            deviceConfigView.dismissLoadingDialog()
            val currentConfig = DeviceConfig(
                    meshNode.node.name,
                    isProxy,
                    isRelay,
                    isFriend,
                    supportLowPower,
                    meshNode.functionality)
            val groupsInSubnet = meshNode.node.subnets.iterator().next().groups.sortedBy { it.name }
            deviceConfigView.setDeviceConfig(meshNode, currentConfig, groupsInSubnet)
        }
    }

    private fun getProxy(): Runnable {
        return Runnable {
            deviceConfigView.setLoadingDialogMessage("Getting proxy status", false)
            configurationControl.checkProxyStatus(object : NodeBehaviourCallback() {
                override fun success(node: Node?, enabled: Boolean) {
                    isProxy = enabled
                    super.success(node, enabled)
                }
            })
        }
    }

    private fun getRelay(): Runnable {
        return Runnable {
            deviceConfigView.setLoadingDialogMessage("Getting relay status", false)
            configurationControl.checkRelayStatus(object : NodeBehaviourCallback() {
                override fun success(node: Node?, enabled: Boolean) {
                    isRelay = enabled
                    super.success(node, enabled)
                }
            })
        }
    }

    private fun getFriend(): Runnable {
        return Runnable {
            deviceConfigView.setLoadingDialogMessage("Getting friend status", false)
            configurationControl.checkFriendStatus(object : NodeBehaviourCallback() {
                override fun success(node: Node?, enabled: Boolean) {
                    isFriend = enabled
                    super.success(node, enabled)
                }
            })
        }
    }

    abstract inner class NodeBehaviourCallback : CheckNodeBehaviourCallback {

        override fun success(node: Node?, enabled: Boolean) {
            takeNextTask()
        }

        override fun error(node: Node?, error: ErrorType?) {
            deviceConfigView.setLoadingDialogMessage(error, true)
            taskList.clear()
        }
    }
}