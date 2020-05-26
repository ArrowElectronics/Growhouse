//
//  ViewModelServices.swift
//  ble_mesh-app
//
//  Created by Lukasz Rzepka on 09.04.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import BluetoothMesh

class ViewModelServices {
    
    static let sharedInstance = ViewModelServices()
    
    let bluetoothManager: BluetoothManager
    let bluetoothMesh: SBMBluetoothMesh
    let bluetoothMeshConfigurationControl: BluetoothMeshConfigurationControl
    let bluetoothMeshElementControl: BluetoothMeshElementControl
    let bluetoothMeshGroupControl: BluetoothMeshGroupControl
    let bluetoothMeshNodeControl: BluetoothMeshNodeControl
    let bluetoothMeshNodeFunctionality: BluetoothMeshNodeFunctionality
    let bluetoothMeshService: BluetoothMeshService
    let bluetoothMeshNetworkManager: BluetoothMeshNetworkManager
    let bluetoothMeshDatabaseExport: BluetoothMeshDatabaseExport
    
    private init() {
        bluetoothMeshConfigurationControl = BluetoothMeshConfigurationControl()
        bluetoothMeshElementControl = BluetoothMeshElementControl()
        bluetoothMeshGroupControl = BluetoothMeshGroupControl()
        bluetoothMeshNodeFunctionality = BluetoothMeshNodeFunctionality()
        bluetoothMeshNodeControl = BluetoothMeshNodeControl(nodeFunctionality: bluetoothMeshNodeFunctionality)
        bluetoothMesh = SBMBluetoothMesh.sharedInstance()
        bluetoothMesh.initialize(SBMBluetoothMeshConfiguration(localVendorModels: [], andLogger: SBMLogger.sharedInstance()))
        bluetoothMeshService = BluetoothMeshService(bluetoothMesh: bluetoothMesh)
        bluetoothManager = BluetoothManager(bluetoothMeshService: bluetoothMeshService)
        bluetoothMeshNetworkManager = BluetoothMeshNetworkManager(meshManager: bluetoothMesh)
        bluetoothMeshDatabaseExport = BluetoothMeshDatabaseExport(bluetoothMesh: bluetoothMesh)
    }
}
