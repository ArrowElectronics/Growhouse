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
    
    let group: SBMMeshGroup
    
    init(_ services: ViewModelServices, group: SBMMeshGroup) {
        self.group = group
        
        super.init(services)
    }
    
    func changeGroupName(to name: String) {
        services.bluetoothMeshService.bleMeshManager.change(group, name: name)
        NotificationCenter.default.post(name: MeshNotifications.GroupsUpdated, object: nil)
    }
}
