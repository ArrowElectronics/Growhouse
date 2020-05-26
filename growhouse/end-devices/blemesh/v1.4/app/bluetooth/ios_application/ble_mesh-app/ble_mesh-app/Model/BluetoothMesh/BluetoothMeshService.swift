//
//  BluetoothMeshService.swift
//  ble_mesh-app
//
//  Created by Lukasz Rzepka on 11.04.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import BluetoothMesh
import CoreBluetooth
import RxCocoa
import RxSwift

typealias ProvisionCallback = (SBMNode?, Error?) -> Void
typealias ProxyConnectionCallback = (Error?) -> Void

class BluetoothMeshService {

    let connected = Variable(false)
    let activeDevice: Variable<BluetoothDevice?> = Variable(nil)
    private let bluetoothMesh: SBMBluetoothMesh
    private var provisionerConnection: SBMProvisionerConnection?
    private var proxyConnection: SBMProxyConnection?
    
    init(bluetoothMesh: SBMBluetoothMesh) {
        self.bluetoothMesh = bluetoothMesh
    }
    
    func provision(device: BluetoothDevice, to subnet: SBMSubnet, completion: ProvisionCallback? = nil) {
        provisionerConnection = SBMProvisionerConnection(for: device, subnet: subnet)
        Logger.log(message: "Provision device: \(device.name()) to subnet: \(subnet.name)")
        provisionerConnection?.provision(asProxy: { [weak self] connection, node, error in
            Logger.log(message: "Provision device: \(node?.name ?? "nil") finished with error: \(error?.localizedDescription ?? "nil")")
            self?.connected.value = error == nil
            completion?(node, error)
        })
    }
    
    func node(for device: BluetoothDevice) -> SBMNode? {
        return bluetoothMesh.networks().flatMap { $0.subnets }.flatMap { $0.nodes }.first { $0.uuid == device.uuid() }
    }
    
    func connect(device: BluetoothDevice, completion: ProxyConnectionCallback? = nil) {
        proxyConnection = SBMProxyConnection(connectableDevice: device)
        Logger.log(message: "Proxy connect to device: \(device.name())")
        proxyConnection?.connect(toProxy: { [weak self] device in
            Logger.log(message: "Success proxy connect to device: \(device.name())")
            completion?(nil)
            self?.connected.value = true
        }, errorCallback: { [weak self] device, error in
            Logger.log(message: "Proxy connect to device: \(device.name()) finished with error: \(error.localizedDescription)")
            self?.connected.value = false
            completion?(error)
        })
    }
    
    func disconnect(device: BluetoothDevice? = nil, completion: ProxyConnectionCallback? = nil) {
        if let device = device {
            proxyConnection = SBMProxyConnection(connectableDevice: device)
        }
        let connection = proxyConnection
        connected.value = false
        proxyConnection = nil
        Logger.log(message: "Proxy disconnect device: \(device?.name() ?? "currently active")")
        connection?.disconnect({ [weak self] (connectableDevice) in
            Logger.log(message: "Success proxy disconnect device: \(device?.name() ?? "currently active")")
            self?.connected.value = false
            completion?(nil)
        }, errorCallback: { (connectableDevice, error) in
            Logger.log(message: "Proxy disconnect from device: \(device?.name() ?? "currently active") finished with error: \(error.localizedDescription)")
            completion?(error)
        })
    }
}
