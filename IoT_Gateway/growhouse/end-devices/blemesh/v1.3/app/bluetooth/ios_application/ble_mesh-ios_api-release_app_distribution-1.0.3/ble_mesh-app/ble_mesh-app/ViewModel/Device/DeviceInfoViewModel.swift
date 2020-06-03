//
//  DeviceInfoViewModel.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 25.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import BluetoothMesh

class DeviceInfoViewModel: BaseViewModel {
    
    var device: SBMMeshNode
    
    init(_ services: ViewModelServices, device: SBMMeshNode) {
        self.device = device
        
        super.init(services)
        
        NotificationCenter.default.addObserver(self, selector: #selector(updateDevice), name: MeshNotifications.NodesUpdated, object: nil)
    }
    
    @objc private func updateDevice() {
        device = services.networkManager.meshNode(withUUID: device.meshDeviceUUID)!
    }
}
