//
//  MeshNetworkManager.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 12/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import BluetoothMesh
import RxSwift
import RxCocoa

class MeshNetworkManager {
    
    private let meshManager: SBMBluetoothMesh
    
    init(meshManager: SBMBluetoothMesh) {
        self.meshManager = meshManager
    }
    
    func getNetworks() -> [SBMMeshNetwork] {
        return meshManager.getNetworks()!
    }
    
    func getNodes(withNetwork network: SBMMeshNetwork) -> [SBMMeshNode] {
        return meshManager.getNodesWith(network)!
    }
    
     func getNodes(withGroup group: SBMMeshGroup) -> [SBMMeshNode] {
        return meshManager.getNodesWith(group)
    }
    
    func createNetwork(name: String) throws -> SBMMeshNetwork {
        let network = try meshManager.createNetwork(name)
        NotificationCenter.default.post(name: MeshNotifications.NetworksUpdated, object: nil)
        
        return network
    }
    
    func removeNetwork(_ network: SBMMeshNetwork) {
        meshManager.remove(network)
        NotificationCenter.default.post(name: MeshNotifications.NetworksUpdated, object: nil)
    }
    
    func createGroup(name: String, network: SBMMeshNetwork) throws -> SBMMeshGroup {
        let group = try meshManager.createGroup(withName: name, in: network)
        NotificationCenter.default.post(name: MeshNotifications.GroupsUpdated, object: nil)

        return group
    }
    
    func removeGroup(_ group: SBMMeshGroup) {
        meshManager.remove(group)
        NotificationCenter.default.post(name: MeshNotifications.GroupsUpdated, object: nil)
    }
    
    func remove(node: SBMMeshNode, fromGroup group: SBMMeshGroup, completion: @escaping (Bool) -> Void) {
        meshManager.remove(node, from: group) { (success) in
            NotificationCenter.default.post(name: MeshNotifications.NodesUpdated, object: nil)
            completion(success)
        }
    }
    
    func getGroups(withNetwork network: SBMMeshNetwork) -> [SBMMeshGroup] {
        return meshManager.getGroupsWith(network)!
    }
    
    func meshNode(withUUID uuid: Data) -> SBMMeshNode? {
        return meshManager.getNetworks()?.map {
            meshManager.getNodesWith($0)?.first { $0.meshDeviceUUID == uuid }
            }.first { $0 != nil } ?? nil
    }
    
    func network(withMeshNode meshNode: SBMMeshNode) -> SBMMeshNetwork? {
        return meshManager.getNetworks()?.first {
            $0.index == meshNode.networkIndex
        }
    }
    
    func group(withMeshNode meshNode: SBMMeshNode) -> SBMMeshGroup? {
        if let network = self.network(withMeshNode: meshNode) {
            return meshManager.getGroupsWith(network)?.first { group in
                guard let configuration = meshNode.configuration else {
                    return false
                }
                return configuration.applicationKeys.contains { $0.key == Int(group.applicationKeyIndex) }
            }
        } else {
            return nil
        }
    }
    
    func network(forNetworkIndex networkIndex: UInt16) -> SBMMeshNetwork? {
        return meshManager.getNetworks()?.first { $0.index == networkIndex }
    }
}
