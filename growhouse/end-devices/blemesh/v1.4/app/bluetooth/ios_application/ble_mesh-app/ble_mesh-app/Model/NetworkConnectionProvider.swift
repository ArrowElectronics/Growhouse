//
//  NetworkConnectionProvider.swift
//  ble_mesh-app
//
//  Created by Lukasz Rzepka on 06.09.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import BluetoothMesh

enum NetworkConnectionProviderError: Error, LocalizedError {
    
    case validPeripheralNotFound
    case peripheralsNotFound
    case proxyConnectionFail
    case proxyConnectionLost
    case noNodesInTheNetwork
    case unknownError(Error)
    
    var errorDescription: String {
        switch self {
        case .validPeripheralNotFound:
            return "Valid peripheral not found"
        case .peripheralsNotFound:
            return "Bluetooth peripherals are not reachable"
        case .proxyConnectionFail:
            return "Not able to establish proxy connection"
        case .proxyConnectionLost:
            return "Lost proxy connection"
        case .noNodesInTheNetwork:
            return "Network is empty"
        case .unknownError(let error):
            return "Error: " + error.localizedDescription
        }
    }
}

typealias NetworkConnectionCallback = (_ device: BluetoothDevice?, _ node: SBMNode?, _ error: NetworkConnectionProviderError?) -> Void

class NetworkConnectionProvider {
    
    private let bluetoothMeshService: BluetoothMeshService
    private let bluetoothManager: BluetoothManager
    private let subnet: SBMSubnet
    private let networkConnectionCallback: NetworkConnectionCallback
    
    init(withSubnet subnet: SBMSubnet, bluetoothMeshService: BluetoothMeshService, bluetoothManager: BluetoothManager, connectionCallback: @escaping NetworkConnectionCallback) {
        self.subnet = subnet
        self.bluetoothMeshService = bluetoothMeshService
        self.bluetoothManager = bluetoothManager
        self.networkConnectionCallback = connectionCallback
    }
    
    func establishConnection()  {
        if subnet.nodes.isEmpty {
            networkConnectionCallback(nil, nil, .noNodesInTheNetwork)
        } else {
            bluetoothManager.startDiscovering(services: [SBMProxyConnection.meshProxyServiceUUID()])
            attemptConnection(after: 2)
        }
    }
    
    private func attemptConnection(after seconds: Int) {
        DispatchQueue.main.asyncAfter(deadline: .now() + .seconds(seconds)) { [weak self] in
            self?.attemptConnection()
        }
    }
    
    private func attemptConnection() {
        bluetoothManager.stopDiscovering()
        let allDevices = bluetoothManager.discoveredDevices.compactMap { $0.value }
        let filteredDevices = validDevices(in: allDevices)
        if let device = deviceWithBestSignal(in: filteredDevices) {
            bluetoothMeshService.connect(device: device) { [weak self] error in
                self?.handleProxyConnection(with: device, error: error)
            }
        } else {
            networkConnectionCallback(nil, nil, .validPeripheralNotFound)
        }
    }
    
    private func validDevices(in devices: [BluetoothDevice]) -> [BluetoothDevice] {
        var validDevices = devices.filter { SBMConnectableDeviceHelper.node($0) != nil }
        
        if validDevices.isEmpty {
            validDevices = devices.filter { SBMConnectableDeviceHelper.node($0, in: subnet) != nil }
        }
        
        return validDevices
    }
    
    private func deviceWithBestSignal(in devices: [BluetoothDevice]) -> BluetoothDevice? {
        return devices.sorted { $0.rssi?.compare($1.rssi!) == .orderedDescending }.first
    }
    
    private func handleProxyConnection(with device: BluetoothDevice, error: Error?) {
        if let error = error {
            networkConnectionCallback(nil, nil, NetworkConnectionProviderError.unknownError(error))
            bluetoothMeshService.disconnect()
        } else {
            networkConnectionCallback(device, SBMConnectableDeviceHelper.node(device), nil)
        }
    }
}
