//
//  GroupViewModel.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 07.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import BluetoothMesh

class GroupViewModel: BaseViewModel {
    
    let network: SBMMeshNetwork
    let group: SBMMeshGroup
    var isConnected = false
    
    init(_ services: ViewModelServices, group: SBMMeshGroup, meshNetwork: SBMMeshNetwork) {
        self.network = meshNetwork
        self.group = group
        
        super.init(services)
    }
}
