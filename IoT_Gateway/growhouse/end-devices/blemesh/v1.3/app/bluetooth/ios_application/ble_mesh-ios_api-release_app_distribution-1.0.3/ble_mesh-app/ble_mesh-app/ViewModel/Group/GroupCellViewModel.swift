//
//  GroupCellViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 26/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import Foundation
import BluetoothMesh

class GroupCellViewModel: BaseViewModel {
    
    let group : SBMMeshGroup
    let name : String
    let info : String
    
    init(services: ViewModelServices, group: SBMMeshGroup) {
        self.group = group
        self.name = group.name
        let network = services.networkManager.network(forNetworkIndex: group.networkIndex)
        
        let deviceCount = services.networkManager.getNodes(withNetwork: network!).filter { (node) -> Bool in
            guard let configuration = node.configuration else {
                return false
            }
            return configuration.applicationKeys.contains { $0.key == Int(group.applicationKeyIndex) }
        }.count
        
        self.info = String.localizedStringWithFormat(NSLocalizedString("%d devices", comment: ""), deviceCount)
        
        super.init(services)
    }
}
