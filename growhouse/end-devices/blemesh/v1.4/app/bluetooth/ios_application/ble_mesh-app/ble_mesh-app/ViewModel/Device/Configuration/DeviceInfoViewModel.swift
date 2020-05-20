//
//  DeviceInfoViewModel.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 25.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import BluetoothMesh

class DeviceInfoViewModel: BaseViewModel {
    
    let device: SBMNode
    
    init(_ services: ViewModelServices, device: SBMNode) {
        self.device = device
        
        super.init(services)
    }
}
