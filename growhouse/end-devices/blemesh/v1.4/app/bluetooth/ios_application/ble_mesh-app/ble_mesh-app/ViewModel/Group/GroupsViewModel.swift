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
    
    var addGroupViewModel: AddGroupViewModel {
        return AddGroupViewModel(services, subnet: subnet)
    }
    let subnet: SBMSubnet
    let groups = Variable([GroupCellViewModel]())
    let canAddGroup = Variable(false)
    
    init(_ services: ViewModelServices, subnet: SBMSubnet) {
        self.subnet = subnet
        super.init(services)

        NotificationCenter.default.addObserver(self, selector: #selector(groupsUpdated(notification:)), name: BluetoothMeshNotifications.GroupsUpdated, object: nil)
        NotificationCenter.default.addObserver(self, selector: #selector(nodesUpdated(notification:)), name: BluetoothMeshNotifications.NodesUpdated, object: nil)
        updateGroups()
    }
    
    func remove(group groupViewModel: GroupCellViewModel, completion: ((_ nodes: [String]?) -> (Void))? = nil) {
        let group = groupViewModel.group
        
        for node in group.nodes {
            services.bluetoothMeshNodeFunctionality.removeActiveFunctionality(for: node)
        }
        
        services.bluetoothMeshNetworkManager.removeGroup(group) {
            completion?($0?.compactMap { $0.name })
            NotificationCenter.default.post(name: BluetoothMeshNotifications.NodesUpdated, object: nil)
        }
    }
    
    func forceRemove(group groupViewModel: GroupCellViewModel) {
        let group = groupViewModel.group
        
        for node in group.nodes {
            services.bluetoothMeshNodeFunctionality.removeActiveFunctionality(for: node)
        }
        
        services.bluetoothMeshNetworkManager.forceRemoveGroup(group)
    }
    
    func updateGroups() {
        groups.value = subnet.groups.map { GroupCellViewModel(services: services, group: $0) }
        canAddGroup.value = subnet.canCreateGroup()
    }
    
    @objc func groupsUpdated(notification: Notification){
        updateGroups()
    }
    
    @objc func nodesUpdated(notification: Notification){
        updateGroups()
    }
}
