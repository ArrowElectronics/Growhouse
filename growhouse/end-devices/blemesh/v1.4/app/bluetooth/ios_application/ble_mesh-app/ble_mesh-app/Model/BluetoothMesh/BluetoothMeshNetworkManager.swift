//
//  BluetoothMeshNetworkManager.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 12/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import BluetoothMesh

class BluetoothMeshNetworkManager {
    
    private let meshManager: SBMBluetoothMesh
    private var network: SBMNetwork?
    
    init(meshManager: SBMBluetoothMesh) {
        self.meshManager = meshManager
        setupNetwork()
    }
    
    private func setupNetwork() {
        network = meshManager.networks().first
        
        if network == nil {
            network = meshManager.createNetwork("Test network")
        }
        
        if network!.subnets.isEmpty {
            createSubnet(name: "Demo network")
        }
        
        if let subnet = network?.subnets.first, subnet.groups.isEmpty {
            createGroup(name: "Demo group", subnet: subnet)
        }
    }
    
    var subnets: [SBMSubnet] {
        return network?.subnets ?? []
    }
    
    func canCreateSubnet() -> Bool {
        return network?.canCreateSubnet() ?? false
    }
    
    func createSubnet(name: String) {
        network?.createSubnet(withName: name)
        NotificationCenter.default.post(name: BluetoothMeshNotifications.NetworksUpdated, object: nil)
    }
    
    func remove(subnet: SBMSubnet, completion: ((_ failedNodes: Set<SBMNode>?) -> (Void))? = nil) {
        guard subnet.nodes.count > 0 else {
            subnet.removeOnlyFromLocalStructure()
            completion?(nil)
            NotificationCenter.default.post(name: BluetoothMeshNotifications.NetworksUpdated, object: nil)
            return
        }
        
        subnet.remove(callback: { _ in
            completion?(nil)
            NotificationCenter.default.post(name: BluetoothMeshNotifications.NetworksUpdated, object: nil)
        }) { subnet, result, error in
            if subnet == nil || subnet?.nodes.count == 0 {
                completion?(nil)
            } else {
                if let nodes = result?.notRemovedNodes.compactMap({ $0.first }) {
                    completion?(Set(nodes))
                } else {
                    completion?(nil)
                }
            }
            NotificationCenter.default.post(name: BluetoothMeshNotifications.NetworksUpdated, object: nil)
        }
    }
    
    func forceRemoveSubnet(_ subnet: SBMSubnet) {
        subnet.removeOnlyFromLocalStructure()
        NotificationCenter.default.post(name: BluetoothMeshNotifications.NetworksUpdated, object: nil)
    }
    
    func createGroup(name: String, subnet: SBMSubnet) {
        _ = subnet.createGroup(withName: name)
        NotificationCenter.default.post(name: BluetoothMeshNotifications.GroupsUpdated, object: nil)
    }
    
    func removeGroup(_ group: SBMGroup, completion: @escaping (_ failedNodes: Set<SBMNode>?) -> (Void)) {
        group.remove(callback: { (group) in
            completion(nil)
            NotificationCenter.default.post(name: BluetoothMeshNotifications.GroupsUpdated, object: nil)
        }, errorCallback: { [weak self] (group, result, error) in
            completion(self?.failedNodes(from: result))
            NotificationCenter.default.post(name: BluetoothMeshNotifications.GroupsUpdated, object: nil)
        })
    }
    
    func forceRemoveGroup(_ group: SBMGroup) {
        group.removeOnlyFromLocalStructure()
        NotificationCenter.default.post(name: BluetoothMeshNotifications.GroupsUpdated, object: nil)
    }
    
    private func failedNodes(from result: SBMGroupRemovalErrorResult?) -> Set<SBMNode>? {
        var failedNodes: Set<SBMNode>?
        if let result = result {
            let nodesFromNodes = result.notUnbindedNodes.compactMap { $0.first }
            let nodesFromSigModels = result.notUnbindedSigModels.compactMap { $0.first.meshElement?.node }
            let nodesFromVendorModels = result.notUnbindedVendorModels.compactMap { $0.first.meshElement?.node }
            let allNodes = nodesFromNodes + nodesFromSigModels + nodesFromVendorModels
            if !allNodes.isEmpty {
                failedNodes = Set(allNodes)
            }
        }
        return failedNodes
    }
    
    func forceRemoveNode(_ node: SBMNode) {
        node.removeOnlyFromLocalStructure()
        NotificationCenter.default.post(name: BluetoothMeshNotifications.NodesUpdated, object: nil)
    }
    
    func node(withUUID uuid: Data) -> SBMNode? {
        return network?.subnets.compactMap { $0.nodes.first { $0.uuid == uuid } }.first
    }
    
    func dropDatabase() {
        meshManager.clearDatabase()
        setupNetwork()
        NotificationCenter.default.post(name: BluetoothMeshNotifications.NetworksUpdated, object: nil)
    }
}
