//
//  GroupCellViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 26/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import BluetoothMesh

class GroupCellViewModel: BaseViewModel {
    
    let group : SBMGroup
    let name : String
    let info : String
    
    init(services: ViewModelServices, group: SBMGroup) {
        self.group = group
        self.name = group.name
        let deviceCount = group.subnet!.nodes.filter { $0.groups.contains(where: { nodeGroup in nodeGroup.appKey == group.appKey })}.count
        self.info = String.localizedStringWithFormat(NSLocalizedString("%d devices", comment: ""), deviceCount)
        
        super.init(services)
    }
}
