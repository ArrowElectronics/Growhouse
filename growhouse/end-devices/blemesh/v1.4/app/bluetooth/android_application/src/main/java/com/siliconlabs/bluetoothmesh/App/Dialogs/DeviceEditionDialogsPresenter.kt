/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Dialogs

import com.siliconlab.bluetoothmesh.adk.ErrorType
import com.siliconlab.bluetoothmesh.adk.configuration_control.ConfigurationControl
import com.siliconlab.bluetoothmesh.adk.data_model.group.Group
import com.siliconlab.bluetoothmesh.adk.data_model.model.Model
import com.siliconlab.bluetoothmesh.adk.data_model.model.SigModel
import com.siliconlab.bluetoothmesh.adk.data_model.node.Node
import com.siliconlab.bluetoothmesh.adk.data_model.node.NodeChangeNameException
import com.siliconlab.bluetoothmesh.adk.functionality_binder.FunctionalityBinder
import com.siliconlab.bluetoothmesh.adk.node_control.NodeControl
import com.siliconlab.bluetoothmesh.adk.notification_control.SubscriptionControl
import com.siliconlab.bluetoothmesh.adk.notification_control.settings.PublicationSettings
import com.siliconlab.bluetoothmesh.adk.notification_control.settings.SubscriptionSettings
import com.siliconlabs.bluetoothmesh.App.Fragments.DeviceList.DeviceListView
import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic
import com.siliconlabs.bluetoothmesh.App.ModelView.MeshNode
import com.siliconlabs.bluetoothmesh.App.Models.DeviceConfig
import com.siliconlabs.bluetoothmesh.App.Models.DeviceFunctionality
import com.siliconlabs.bluetoothmesh.App.Models.MeshNodeManager
import com.siliconlabs.bluetoothmesh.App.Utils.Converters
import java.util.concurrent.Executors

class DeviceEditionDialogsPresenter(val dialog: DeviceEditionDialogs, val parentView: ParentView, val meshLogic: MeshLogic, val deviceFunctionalityDb: MeshNodeManager) : DeviceEditionDialogs.DeviceEditionDialogsListener {
    private val TAG: String = javaClass.canonicalName

    interface ParentView {
        fun returnToNetworkList()
        fun refreshList()
        fun showToast(message: TOAST_MESSAGE)
    }

    enum class TOAST_MESSAGE {
        ERROR_DELETE_DEVICE,
        ERROR_MISSING_GROUP
    }

    private val taskExecutor = Executors.newSingleThreadScheduledExecutor()
    private val taskList = mutableListOf<Runnable>()

    private var exitFromNetworkOnFinish: Boolean = false

    override fun dismiss() {
    }

    fun showDeviceConfigDialog(deviceInfo: MeshNode) {
        dialog.showDeviceConfigDialog(deviceInfo)
    }

    override fun deleteDevices(devicesInfo: List<MeshNode>) {
        val nodes = moveProxyNodeToEnd(devicesInfo)

        nodes.forEach { device ->
            val configurationControl = ConfigurationControl(device.node)
            taskList.add(factoryResetDevice(configurationControl))
        }

        startTasks()
    }

    fun moveProxyNodeToEnd(devicesInfo: List<MeshNode>): List<MeshNode> {
        val nodes = mutableListOf<MeshNode>()
        nodes.addAll(devicesInfo)

        val proxyNode = nodes.find { it.node.isConnectedAsProxy }
        proxyNode?.let {
            nodes.remove(it)
            nodes.add(it)
        }

        return nodes
    }

    override fun updateDeviceConfig(meshNode: MeshNode, deviceConfig: DeviceConfig, exitFromNetworkOnFinish: Boolean) {
        this.exitFromNetworkOnFinish = exitFromNetworkOnFinish
        val configurationControl = ConfigurationControl(meshNode.node)

        val currentGroup = meshNode.node.groups.firstOrNull()

        deviceConfig.name?.let {
            taskList.add(changeName(meshNode, it))
        }
        deviceConfig.proxy?.let {
            taskList.add(changeProxy(configurationControl, it))
        }
        deviceConfig.relay?.let {
            taskList.add(changeRelay(configurationControl, it))
        }
        deviceConfig.friend?.let {
            taskList.add(changeFriend(configurationControl, it))
        }
        deviceConfig.group?.let {
            val newFunctionality = deviceConfig.functionality ?: meshNode.functionality

            if (currentGroup != null && meshNode.functionality != DeviceFunctionality.FUNCTIONALITY.Unknown) {
                taskList.add(processUnbindModelFromGroup(meshNode.node, currentGroup, meshNode.functionality))
                taskList.add(unbindFromGroup(meshNode.node, currentGroup))
            }

            taskList.add(bindToGroup(meshNode.node, it))
            taskList.add(processBindModelToGroup(meshNode.node, it, newFunctionality))
            taskList.add(changeFunctionality(meshNode, newFunctionality))
        }
        deviceConfig.functionality?.let {
            if (deviceConfig.group != null) {
                return@let
            }
            val newGroup = deviceConfig.group ?: currentGroup

            if (currentGroup != null && meshNode.functionality != DeviceFunctionality.FUNCTIONALITY.Unknown) {
                taskList.add(processUnbindModelFromGroup(meshNode.node, currentGroup, meshNode.functionality))
            }
            if (newGroup != null) {
                taskList.add(processBindModelToGroup(meshNode.node, newGroup, it))
                taskList.add(changeFunctionality(meshNode, it))
            }
        }

        startTasks()
    }

    fun startTasks() {
        if (taskList.isNotEmpty()) {
            dialog.showLoadingDialog()

            takeNextTask()
        }
    }

    fun takeNextTask() {
        if (taskList.isNotEmpty()) {
            val task = taskList.first()
            taskList.remove(task)
            taskExecutor.execute(task)
        } else {
            dialog.dismissLoadingDialog()
            parentView.refreshList()
        }
    }

    fun processBindModelToGroup(node: Node, group: Group, functionality: DeviceFunctionality.FUNCTIONALITY): Runnable {
        return Runnable {
            DeviceFunctionality.getSigModels(node, functionality).forEach { model ->
                if (group.sigModels.contains(model)) {
                    return@forEach
                }

                if (model.isSupportPublish) {
                    taskList.add(0, setPublicationSettings(model, group))
                }
                if (model.isSupportSubscribe) {
                    taskList.add(0, addSubscriptionSettings(model, group))
                }
                taskList.add(0, bindModelToGroup(model, group))
            }
            takeNextTask()
        }
    }

    fun processUnbindModelFromGroup(node: Node, group: Group, functionality: DeviceFunctionality.FUNCTIONALITY): Runnable {
        return Runnable {
            DeviceFunctionality.getSigModels(node, functionality).forEach { model ->
                if (!group.sigModels.contains(model)) {
                    return@forEach
                }

                taskList.add(0, unbindModelFromGroup(model, group))
                if (model.isSupportSubscribe) {
                    taskList.add(0, removeSubscriptionSettings(model, group))
                }
                if (model.isSupportPublish) {
                    taskList.add(0, clearPublicationSettings(model))
                }
            }
            takeNextTask()
        }
    }

    fun changeName(meshNode: MeshNode, name: String): Runnable {
        return Runnable {
            try {
                meshNode.node.name = name
                parentView.refreshList()
                takeNextTask()
            } catch (e: NodeChangeNameException) {
                dialog.updateLoadingDialogMessage(ErrorType(ErrorType.TYPE.CANNOT_SAVE_TO_DATABASE))
                taskList.clear()
            }
        }
    }

    fun changeFunctionality(meshNode: MeshNode, functionality: DeviceFunctionality.FUNCTIONALITY): Runnable {
        return Runnable {
            try {
                deviceFunctionalityDb.updateNodeFunc(meshNode, functionality)
                parentView.refreshList()
                takeNextTask()
            } catch (e: NodeChangeNameException) {
                dialog.updateLoadingDialogMessage(ErrorType(ErrorType.TYPE.CANNOT_SAVE_TO_DATABASE))
                taskList.clear()
            }
        }
    }

    fun changeProxy(configurationControl: ConfigurationControl, enabled: Boolean): Runnable {
        return Runnable {
            if (enabled) {
                dialog.updateLoadingDialogMessage(DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_PROXY_ENABLING)
            } else {
                dialog.updateLoadingDialogMessage(DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_PROXY_DISABLING)
            }
            configurationControl.setProxy(enabled, object : SetNodeBehaviourCallback() {
                override fun error(node: Node?, error: ErrorType?) {
                    dialog.updateLoadingDialogMessage(error!!)
                    super.error()
                }
            })
        }
    }

    fun changeRelay(configurationControl: ConfigurationControl, enabled: Boolean): Runnable {
        return Runnable {
            if (enabled) {
                dialog.updateLoadingDialogMessage(DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_RELAY_ENABLING)
            } else {
                dialog.updateLoadingDialogMessage(DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_RELAY_DISABLING)
            }
            configurationControl.setRelay(enabled, 0, 0, object : SetNodeBehaviourCallback() {
                override fun error(node: Node?, error: ErrorType?) {
                    dialog.updateLoadingDialogMessage(error!!)
                    super.error()
                }
            })
        }
    }

    fun changeFriend(configurationControl: ConfigurationControl, enabled: Boolean): Runnable {
        return Runnable {
            if (enabled) {
                dialog.updateLoadingDialogMessage(DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_FRIEND_ENABLING)
            } else {
                dialog.updateLoadingDialogMessage(DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_FRIEND_DISABLING)
            }
            configurationControl.setFriend(enabled, object : SetNodeBehaviourCallback() {
                override fun error(node: Node?, error: ErrorType?) {
                    dialog.updateLoadingDialogMessage(error!!)
                }
            })
        }
    }

    fun factoryResetDevice(configurationControl: ConfigurationControl): Runnable {
        return Runnable {
            dialog.updateLoadingDialogMessage(DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_DEVICE_DELETING)
            configurationControl.factoryReset(object : FactoryResetCallback() {
                override fun success(node: Node?) {
                    node?.let {
                        val meshNode = deviceFunctionalityDb.getMeshNode(it)
                        deviceFunctionalityDb.removeNodeFunc(meshNode)
                    }
                    super.success(node)
                }

                override fun error(node: Node?, error: ErrorType?) {
                    dialog.dismissLoadingDialog()
                    dialog.showDeleteDeviceLocallyDialog(error!!, node!!)
                    super.error()
                }
            })
        }
    }

    override fun deleteDeviceLocally(node: Node) {
        node.removeOnlyFromLocalStructure()
        parentView.refreshList()
    }

    fun bindToGroup(node: Node, group: Group): Runnable {
        return Runnable {
            val nodeControl = NodeControl(node)

            dialog.updateLoadingDialogMessage(DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_ADDING_TO_GROUP, group.name)
            nodeControl.bind(group, object : NodeControlCallback() {
                override fun error(errorType: ErrorType?) {
                    dialog.updateLoadingDialogMessage(errorType!!)
                    super.error()
                }
            })
        }
    }

    fun unbindFromGroup(node: Node, group: Group): Runnable {
        return Runnable {
            val nodeControl = NodeControl(node)

            dialog.updateLoadingDialogMessage(DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_REMOVING_FROM_GROUP, group.name)
            nodeControl.unbind(group, object : NodeControlCallback() {
                override fun error(errorType: ErrorType?) {
                    dialog.updateLoadingDialogMessage(errorType!!)
                    super.error()
                }
            })
        }
    }

    fun bindModelToGroup(model: SigModel, group: Group): Runnable {
        return Runnable {
            val functionalityBinder = FunctionalityBinder(group)

            dialog.updateLoadingDialogMessage(DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_MODEL_ADDING, Converters.shortString(model.id))
            functionalityBinder.bindModel(model, object : FunctionalityBinderCallback() {
                override fun error(succeededModels: MutableList<Model>?, group: Group?, errorType: ErrorType?) {
                    dialog.updateLoadingDialogMessage(errorType!!)
                    super.error()
                }
            })
        }
    }

    fun unbindModelFromGroup(model: Model, group: Group): Runnable {
        return Runnable {
            val functionalityBinder = FunctionalityBinder(group)

            dialog.updateLoadingDialogMessage(DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_MODEL_REMOVING, Converters.shortString(model.id))
            functionalityBinder.unbindModel(model, object : FunctionalityBinderCallback() {
                override fun error(succeededModels: MutableList<Model>?, group: Group?, errorType: ErrorType?) {
                    dialog.updateLoadingDialogMessage(errorType!!)
                    super.error()
                }
            })
        }
    }

    fun addSubscriptionSettings(model: SigModel, group: Group): Runnable {
        return Runnable {
            val subscriptionControl = SubscriptionControl(model)
            val subscriptionSettings = SubscriptionSettings(group)

            dialog.updateLoadingDialogMessage(DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_SUBSCRIPTION_ADDING, Converters.shortString(model.id))
            subscriptionControl.addSubscriptionSettings(subscriptionSettings, object : SubscriptionSettingsCallback() {
                override fun error(meshModel: SigModel?, errorType: ErrorType?) {
                    dialog.updateLoadingDialogMessage(errorType!!)
                    super.error(meshModel, errorType)
                }
            })
        }
    }

    fun removeSubscriptionSettings(model: SigModel, group: Group): Runnable {
        return Runnable {
            val subscriptionSettings = SubscriptionSettings(group)
            val subscriptionControl = SubscriptionControl(model)

            dialog.updateLoadingDialogMessage(DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_SUBSCRIPTION_REMOVING, Converters.shortString(model.id))
            subscriptionControl.removeSubscriptionSettings(subscriptionSettings, object : SubscriptionSettingsCallback() {
                override fun error(meshModel: SigModel?, errorType: ErrorType?) {
                    dialog.updateLoadingDialogMessage(errorType!!)
                    super.error(meshModel, errorType)
                }
            })
        }
    }

    fun setPublicationSettings(model: SigModel, group: Group): Runnable {
        return Runnable {
            val subscriptionControl = SubscriptionControl(model)
            val publicationSettings = PublicationSettings(group)
            publicationSettings.ttl = 5

            dialog.updateLoadingDialogMessage(DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_PUBLICATION_SETTING, Converters.shortString(model.id))
            subscriptionControl.setPublicationSettings(publicationSettings, object : PublicationSettingsCallback() {
                override fun error(meshModel: SigModel?, errorType: ErrorType?) {
                    dialog.updateLoadingDialogMessage(errorType!!)
                    super.error(meshModel, errorType)
                }
            })
        }
    }

    fun clearPublicationSettings(model: SigModel): Runnable {
        return Runnable {
            val subscriptionControl = SubscriptionControl(model)

            dialog.updateLoadingDialogMessage(DeviceListView.LOADING_DIALOG_MESSAGE.CONFIG_PUBLICATION_CLEARING, Converters.shortString(model.id))
            subscriptionControl.clearPublicationSettings(object : PublicationSettingsCallback() {
                override fun error(meshModel: SigModel?, errorType: ErrorType?) {
                    dialog.updateLoadingDialogMessage(errorType!!)
                    super.error(meshModel, errorType)
                }
            })
        }
    }

    abstract inner class PublicationSettingsCallback : com.siliconlab.bluetoothmesh.adk.notification_control.PublicationSettingsCallback {
        override fun success(meshModel: SigModel?, publicationSettings: PublicationSettings?) {
            takeNextTask()
        }

        override fun error(meshModel: SigModel?, errorType: ErrorType?) {
            parentView.refreshList()
            taskList.clear()
        }
    }

    abstract inner class SubscriptionSettingsSetCallback : com.siliconlab.bluetoothmesh.adk.notification_control.SubscriptionSettingsSetCallback {
        override fun success(meshModel: SigModel?, subscriptionSettings: MutableSet<SubscriptionSettings>?) {
            takeNextTask()
        }

        override fun error(meshModel: SigModel?, errorType: ErrorType?) {
            parentView.refreshList()
            taskList.clear()
        }
    }

    abstract inner class SubscriptionSettingsCallback : com.siliconlab.bluetoothmesh.adk.notification_control.SubscriptionSettingsCallback {
        override fun success(meshModel: SigModel?, subscriptionSettings: SubscriptionSettings?) {
            takeNextTask()
        }

        override fun error(meshModel: SigModel?, errorType: ErrorType?) {
            parentView.refreshList()
            taskList.clear()
        }
    }

    abstract inner class FunctionalityBinderCallback : com.siliconlab.bluetoothmesh.adk.functionality_binder.FunctionalityBinderCallback {
        override fun succeed(succeededModels: MutableList<Model>?, group: Group?) {
            takeNextTask()
        }

        fun error() {
            parentView.refreshList()
            taskList.clear()
        }
    }

    abstract inner class FactoryResetCallback : com.siliconlab.bluetoothmesh.adk.configuration_control.FactoryResetCallback {
        override fun success(node: Node?) {
            takeNextTask()
        }

        fun error() {
            parentView.refreshList()
            taskList.clear()
        }
    }

    abstract inner class NodeControlCallback : com.siliconlab.bluetoothmesh.adk.node_control.NodeControlCallback {
        override fun succeed() {
            takeNextTask()
        }

        fun error() {
            parentView.refreshList()
            taskList.clear()
        }
    }

    abstract inner class SetNodeBehaviourCallback : com.siliconlab.bluetoothmesh.adk.configuration_control.SetNodeBehaviourCallback {
        override fun success(node: Node?, enabled: Boolean) {
            takeNextTask()
        }

        fun error() {
            parentView.refreshList()
            taskList.clear()
        }
    }

}