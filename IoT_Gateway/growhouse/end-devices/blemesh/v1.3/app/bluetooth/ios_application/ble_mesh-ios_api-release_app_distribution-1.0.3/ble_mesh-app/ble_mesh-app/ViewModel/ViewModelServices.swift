//
//  ViewModelServices.swift
//  ble_mesh-app
//
//  Created by Lukasz Rzepka on 09.04.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import Foundation

class ViewModelServices {
    static let sharedInstance = ViewModelServices()
    
    let peripheralDiscovery: PeripheralDiscovery
    let bluetoothMeshService: BluetoothMeshService
    let networkManager: MeshNetworkManager
    
    init() {
        self.peripheralDiscovery = PeripheralDiscovery()
        self.bluetoothMeshService = BluetoothMeshService()
        self.networkManager = MeshNetworkManager(meshManager: bluetoothMeshService.bleMeshManager)
        self.bluetoothMeshService.peripheralDiscovery = self.peripheralDiscovery
        self.bluetoothMeshService.networkManager = self.networkManager
    }
}
