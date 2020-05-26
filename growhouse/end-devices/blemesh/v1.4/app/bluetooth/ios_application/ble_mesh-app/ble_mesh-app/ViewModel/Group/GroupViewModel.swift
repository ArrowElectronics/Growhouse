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
    
    let subnet: SBMSubnet
    let group: SBMGroup
    
    init(_ services: ViewModelServices, group: SBMGroup, subnet: SBMSubnet) {
        self.subnet = subnet
        self.group = group
        
        super.init(services)
    }
}
