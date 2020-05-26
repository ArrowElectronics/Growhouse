//
//  GroupConfigurationViewModel.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 05.06.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import BluetoothMesh

class GroupConfigurationViewModel: BaseViewModel {
    
    let groupName: String
    let groupId: String
    let subnetName: String
    private let group: SBMGroup
    
    init(_ services: ViewModelServices, group: SBMGroup) {
        self.group = group
        groupName = group.name
        groupId = group.appKey.rawKey.hexEncodedString()
        subnetName = group.subnet!.name
        
        super.init(services)
    }
    
    func changeGroupName(to name: String) {
        group.name = name
        NotificationCenter.default.post(name: BluetoothMeshNotifications.GroupsUpdated, object: nil)
    }
}
