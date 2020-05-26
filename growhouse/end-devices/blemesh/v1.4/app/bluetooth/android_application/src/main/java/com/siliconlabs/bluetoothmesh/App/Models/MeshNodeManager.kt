/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Models

import com.siliconlab.bluetoothmesh.adk.data_model.group.Group
import com.siliconlab.bluetoothmesh.adk.data_model.node.Node
import com.siliconlab.bluetoothmesh.adk.data_model.subnet.Subnet
import com.siliconlabs.bluetoothmesh.App.Database.DeviceFunctionalityDb
import com.siliconlabs.bluetoothmesh.App.ModelView.MeshNode

/**
 * @author Comarch S.A.
 */
class MeshNodeManager(private val deviceFunctionalityDb: DeviceFunctionalityDb) {

    private val meshNodes = mutableMapOf<Node, MeshNode>()

    fun getMeshNode(node: Node): MeshNode {
        return wrapNode(node)
    }

    fun getMeshNodes(subnet: Subnet): Set<MeshNode> {
        return wrapNodes(subnet.nodes)
    }

    fun getMeshNodes(group: Group): Set<MeshNode> {
        return wrapNodes(group.nodes)
    }

    private fun wrapNodes(nodes: Set<Node>): Set<MeshNode> {
        val result = mutableSetOf<MeshNode>()
        nodes.forEach { node ->
            result.add(wrapNode(node))
        }
        return result
    }

    private fun wrapNode(node: Node): MeshNode {
        var meshNode: MeshNode? = meshNodes[node]
        if (meshNode == null) {
            meshNode = MeshNode(node)
            meshNode.functionality = deviceFunctionalityDb.getFunctionality(node)
            meshNodes[node] = meshNode
        }

        return meshNode
    }

    fun updateNodeFunc(meshNode: MeshNode, functionality: DeviceFunctionality.FUNCTIONALITY) {
        meshNode.functionality = functionality
        if (functionality != DeviceFunctionality.FUNCTIONALITY.Unknown) {
            deviceFunctionalityDb.saveFunctionality(meshNode.node, functionality)
        } else {
            deviceFunctionalityDb.removeFunctionality(meshNode.node)
        }
    }

    fun removeNodeFunc(meshNode: MeshNode) {
        meshNode.functionality = DeviceFunctionality.FUNCTIONALITY.Unknown
        deviceFunctionalityDb.removeFunctionality(meshNode.node)
    }
}