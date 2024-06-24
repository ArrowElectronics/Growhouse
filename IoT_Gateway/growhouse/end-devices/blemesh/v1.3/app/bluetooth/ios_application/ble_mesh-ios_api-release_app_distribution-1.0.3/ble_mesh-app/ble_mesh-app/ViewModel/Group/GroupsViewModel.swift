//
//  GroupsViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 23/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import RxSwift
import BluetoothMesh
import SVProgressHUD

class GroupsViewModel: BaseViewModel {
    let network : SBMMeshNetwork
    let groups = Variable<[GroupCellViewModel]>([])
    var isConnected = false
    
    init(_ services: ViewModelServices, meshNetwork: SBMMeshNetwork) {
        self.network = meshNetwork
        super.init(services)

        NotificationCenter.default.addObserver(self, selector: #selector(groupsUpdated(notification:)), name: MeshNotifications.GroupsUpdated, object: nil)
        NotificationCenter.default.addObserver(self, selector: #selector(nodesUpdated(notification:)), name: MeshNotifications.NodesUpdated, object: nil)
        updateGroups()
    }
    
    func addGroupViewModel() -> AddGroupViewModel {
        return AddGroupViewModel(services, network: network)
    }
    
    func remove(group groupViewModel: GroupCellViewModel) {
        let group = groupViewModel.group
        let nodes = services.networkManager.getNodes(withGroup: group)
        
        SVProgressHUD.show(withStatus: "Removing group")
        
        remove(nodes: nodes, fromGroup: group) { [weak self] (success) in
            if success {
                self?.services.networkManager.removeGroup(group)
                SVProgressHUD.showSuccess(withStatus: "Group removed")
                SVProgressHUD.dismiss(withDelay: 2)
            } else {
                SVProgressHUD.showError(withStatus: "Error removing group")
                SVProgressHUD.dismiss(withDelay: 2)
            }
        }
    }
    
    func remove(nodes: [SBMMeshNode], fromGroup group: SBMMeshGroup, completion: @escaping (Bool) -> Void) {
        if nodes.count == 0 {
            completion(true)
            return
        }
        
        let nodesHead = nodes.first!
        let nodesTail = Array(nodes[1...])
        
        services.networkManager.remove(node: nodesHead, fromGroup: group) { [weak self] (success) in
            if success {
                self?.remove(nodes: nodesTail, fromGroup: group, completion: completion)
            } else {
                completion(false)
            }
        }
    }
    
    func updateGroups() {
        groups.value = services.networkManager.getGroups(withNetwork: network).map { GroupCellViewModel(services: services, group: $0) }
    }
    
    @objc func groupsUpdated(notification: Notification){
        updateGroups()
    }
    
    @objc func nodesUpdated(notification: Notification){
        updateGroups()
    }
}
