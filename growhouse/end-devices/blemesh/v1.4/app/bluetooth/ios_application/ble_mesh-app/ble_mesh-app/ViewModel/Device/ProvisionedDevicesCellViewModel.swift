//
//  ProvisionedDevicesCellViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 20/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import BluetoothMesh

class ProvisionedDevicesCellViewModel {
    
    let node: SBMNode
    let name: String?
    
    init(node: SBMNode) {
        self.node = node
        self.name = node.name
    }
}
