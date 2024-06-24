//
//  BluetoothMeshService.swift
//  ble_mesh-app
//
//  Created by Lukasz Rzepka on 11.04.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import BluetoothMesh
import CoreBluetooth
import SVProgressHUD

enum BluetoothMeshServiceError: Error, LocalizedError {
    case subscriptionError
    case peripheralConnectionError
    case provisionError
    case connectToProxyError
    case noCharacteristicError
    case peripheralNotFound
    
    var errorDescription: String? {
        switch self {
        case .subscriptionError:
            return "Subscription fail"
        case .peripheralConnectionError:
            return "Peripheral connection fail"
        case .provisionError:
            return "Provision fail"
        case .connectToProxyError:
            return "Proxy connection fail"
        case .noCharacteristicError:
            return "Did not find characteristic"
        case .peripheralNotFound:
            return "Peripheral not found"
        }
    }
}


typealias ProvisionCallback = (SBMMeshNode?, BluetoothMeshServiceError?) -> Void
typealias ProxyConnectionCallback = (Int?, BluetoothMeshServiceError?) -> Void
typealias PeripheralConnectionCallback = (BluetoothMeshServiceError?) -> Void

class BluetoothMeshService: NSObject, SBMBluetoothAdapterDelegate, PeripheralConnectionDelegate {

    var activePeripheralConnection: PeripheralConnection?
    var bleMeshManager: SBMBluetoothMesh!
    var peripheralDiscovery: PeripheralDiscovery?
    var networkManager: MeshNetworkManager?
    let requestConnectionTimeout = 10
    
    override init() {
        super.init()
        bleMeshManager = SBMBluetoothMesh()
        bleMeshManager.initialise()
        bleMeshManager.bluetoothAdapter.delegate = self;
    }
    
    func provisionPeripheralConnection(_ peripheralConnection: PeripheralConnection, withName name:String, toNetworkIndex: UInt16, onProvisionComplete: ProvisionCallback!) {
        guard let meshNetwork: SBMMeshNetwork = self.networkManager?.network(forNetworkIndex: toNetworkIndex) else {
            return
        }
        
        establishPeripheralConnection(peripheralConnection, meshServiceUUID: meshProvisioningServiceUUID) { [weak self] error in
            if error != nil {
                onProvisionComplete(nil, error)
                return
            }
            
            self?.bleMeshManager.provisionDevice(meshNetwork, deviceName: name, deviceUUID: (peripheralConnection.meshDeviceUUID)!, gattHandle: (peripheralConnection.gattHandle)) { (node, error) in
                print("Did provision peripheral: ", peripheralConnection, "gattHandle:", peripheralConnection.gattHandle, "node uuid:", node?.meshDeviceUUID ?? "", "to completion:", onProvisionComplete, "error:", error ?? "")
                
                if error != nil {
                    onProvisionComplete(nil, BluetoothMeshServiceError.provisionError)
                } else {
                    NotificationCenter.default.post(name: MeshNotifications.NodesUpdated, object: nil)
                    onProvisionComplete(node, nil)
                }
            }
        }
    }
    
    func startProxyConnectionWith(_ peripheralConnection: PeripheralConnection, onProxyConnectionComplete: ProxyConnectionCallback!) {
        establishPeripheralConnection(peripheralConnection, meshServiceUUID: meshProxyServiceUUID) { [weak self] error in
            if error != nil {
                onProxyConnectionComplete(peripheralConnection.gattHandle, BluetoothMeshServiceError.peripheralConnectionError)
                return
            }

            self?.bleMeshManager.connectToProxy(withGattHandle: peripheralConnection.gattHandle, completion: { (gattHandle, success) in
                if success {
                    onProxyConnectionComplete(gattHandle, nil)
                    print("SUCCESS CONNECT TO PROXY")
                } else {
                    onProxyConnectionComplete(gattHandle, BluetoothMeshServiceError.connectToProxyError)
                }
            })
        }
    }
    
    func establishPeripheralConnection(_ peripheralConnection: PeripheralConnection, meshServiceUUID:String, connectionCallback: PeripheralConnectionCallback!) {
        guard let peripheralConnectionTuple = peripheralDiscovery?.discoveredPeripherals.first(where: { $0.value.gattHandle == peripheralConnection.gattHandle }) else {
            connectionCallback(BluetoothMeshServiceError.peripheralNotFound)
            return
        }

        peripheralConnection.delegate = self
        peripheralConnection.peripheral.delegate = peripheralConnection
        
        peripheralDiscovery?.connectPeripheral(peripheralConnectionTuple.value, timeout: requestConnectionTimeout, connectionCallback: { [weak self] peripheralConnection, error in
            if error != nil {
                connectionCallback(BluetoothMeshServiceError.peripheralConnectionError)
                return
            }
            
            guard let peripheralConnection = peripheralConnection else {
                connectionCallback(BluetoothMeshServiceError.peripheralConnectionError)
                return
            }
            
            self?.activePeripheralConnection = peripheralConnection
            self?.bleMeshManager.bluetoothAdapter.updateMTU(peripheralConnection.peripheral.maximumWriteValueLength(for: .withoutResponse))
            connectionCallback(nil)
        })
    }
    
    func deviceCompositionData(for node: SBMMeshNode, completion: @escaping (SBMDeviceCompositionData?, Error?) -> Void) {
        self.bleMeshManager.deviceCompositionData(for: node, completion: completion)
    }
    
    func didUpdateValueFor(_ peripheralConnection: PeripheralConnection, _ characteristic: CBCharacteristic) {
        if (characteristic.uuid.uuidString == characteristicMeshProvisioningDataOutUUID || characteristic.uuid.uuidString == characteristicMeshProxyDataOutUUID) {
            print("Receive data from peripheral, ", peripheralConnection, "gattHandle: ", peripheralConnection.gattHandle, "characteristic: ", characteristic)
            bleMeshManager.bluetoothAdapter.writeData(toGattHandle: peripheralConnection.gattHandle, byte: characteristic.value!)
        }
    }
    
    func bluetoothAdapter(_ bluetoothAdapter: SBMBluetoothAdapter, gattRequestToGattHandle gattHandle: Int, _ gattServiceType: SBMGattServiceType) {
        switch gattServiceType {
        case .proxy:
            self.subscribeToProxyCharacteristic { [weak self] success in
                guard success, let activeConnection = self?.activePeripheralConnection else {
                    self?.closeMeshNodeConnectionWith(self?.activePeripheralConnection)
                    return
                }
                
                self?.bleMeshManager.bluetoothAdapter.updateMTU(activeConnection.peripheral.maximumWriteValueLength(for: .withoutResponse))
                self?.bleMeshManager.bluetoothAdapter.connectGatt(gattHandle)
            }
        case .provisioning:
            self.subscribeToProvisioningCharacteristic { [weak self] success in
                guard success, let activeConnection = self?.activePeripheralConnection else {
                    self?.closeMeshNodeConnectionWith(self?.activePeripheralConnection)
                    return
                }
                
                self?.bleMeshManager.bluetoothAdapter.updateMTU(activeConnection.peripheral.maximumWriteValueLength(for: .withoutResponse))
                self?.bleMeshManager.bluetoothAdapter.connectGatt(gattHandle)
            }
        }
    }
    
    func bluetoothAdapter(_ bluetoothAdapter: SBMBluetoothAdapter, disconnectionRequestToGattHandle gattHandle: Int) {
        closeMeshNodeConnectionWith(activePeripheralConnection)
    }
    
    func bluetoothAdapter(_ bluetoothAdapter: SBMBluetoothAdapter, gattWriteToGattHandle gattHandle: Int, data: Data, serviceUUID: String, characteristic characteristicUUID: String) {
        guard let peripheralConnection = activePeripheralConnection else {
            print("Did not found peripheral for gattHandle: ", gattHandle)
            return
        }
        
        if let writeCharacteristic: CBCharacteristic = peripheralConnection.characteristicForUUID(characteristicUUID, serviceUUID:serviceUUID) {
            print("Send data to peripheral, ", peripheralConnection, "connection state:", peripheralConnection.peripheral.state.rawValue, " serviceUUID: ", serviceUUID, " characteristicUUID: ", characteristicUUID, " data: ", NSData(data:data))
            peripheralConnection.peripheral.writeValue(data, for: writeCharacteristic, type: .withoutResponse)
        } else {
            //TODO
        }
    }
    
    func closeMeshNodeConnectionWith(_ proxyConnection: PeripheralConnection!) {
        if proxyConnection.peripheral.state == .disconnected {
            self.bleMeshManager.bluetoothAdapter.disconnectGatt(proxyConnection.gattHandle)
            self.activePeripheralConnection = nil
        } else {
            peripheralDiscovery?.closePeripheralConnection(proxyConnection, completion: { [weak self] in
                self?.bleMeshManager.bluetoothAdapter.disconnectGatt(proxyConnection.gattHandle)
                self?.activePeripheralConnection = nil
            })
        }
    }
    
    func subscribeToProxyCharacteristic(completion: ((_ success: Bool) -> (Void))!) {
        guard let activeConnection = activePeripheralConnection else {
            completion(false)
            return
        }
        
        activeConnection.discoverCharacteristicsForService(meshProxyServiceUUID, withCallback: { (serviceUUIDcallback, success) in
            guard success else {
                completion(false)
                print("Did not find characteristics: ",  "for service: ", meshProxyServiceUUID)
                return
            }
            
            activeConnection.subscribeCharacteristics(characteristicMeshProxyDataOutUUID, forServiceUUID: meshProxyServiceUUID, withCallback: { (success) in
                completion(success)
            })
        })
    }
    
    func subscribeToProvisioningCharacteristic(completion: ((_ success: Bool) -> (Void))!) {
        guard let activeConnection = activePeripheralConnection else {
            completion(false)
            return
        }
        
        activeConnection.discoverCharacteristicsForService(meshProvisioningServiceUUID, withCallback: { (serviceUUIDcallback, success) in
            guard success else {
                completion(false)
                print("Did not find characteristics: ",  "for service: ", meshProvisioningServiceUUID)
                return
            }
            
            activeConnection.subscribeCharacteristics(characteristicMeshProvisioningDataOutUUID, forServiceUUID: meshProvisioningServiceUUID, withCallback: { success in
                completion(success)
            })
        })
    }
}
