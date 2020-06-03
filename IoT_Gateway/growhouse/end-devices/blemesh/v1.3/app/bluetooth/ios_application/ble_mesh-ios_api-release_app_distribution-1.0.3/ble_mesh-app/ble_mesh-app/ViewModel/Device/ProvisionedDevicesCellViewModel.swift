//
//  ProvisionedDevicesCellViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 20/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import Foundation
import BluetoothMesh

class ProvisionedDevicesCellViewModel {
    let meshNode: SBMMeshNode
    let name: String?
    
    init(meshNode: SBMMeshNode) {
        self.meshNode = meshNode
        self.name = meshNode.name
    }
}
