//
//  BluetoothService.swift
//  ble_mesh-app
//
//  Created by Lukasz Rzepka on 09.04.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import CoreBluetooth
import RxCocoa
import RxSwift

enum PeripheralConnectionError: Error {
    case didFailToConnect
    case didLoseConnection
    case timeout
}

typealias ConnectionCallback = (_ peripheralConnection: PeripheralConnection?, _ peripheralConnectionError: PeripheralConnectionError?) -> ()

@objcMembers class PeripheralDiscovery: NSObject, CBCentralManagerDelegate, CBPeripheralDelegate {
    
    private var centralManager: CBCentralManager?
    private var currentPeripheralConnection: PeripheralConnection?
    dynamic var discoveredPeripherals = [UUID: PeripheralConnection]()
    private var connectionCallbacks = [UUID: ConnectionCallback]()
    private var disconnectCallback: (() -> ())?
    private(set) var bluetoothConnected = false
    
    func startBluetoothService() {
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }
    
    func stopDiscovery() {
        centralManager?.stopScan()
    }
    
    func startDiscovery(forServiceUUIDs serviceUUIDs: [String]?) {
        discoverPeripheralDevices(serviceUUIDs: serviceUUIDs)
    }
    
    func discoverPeripheralDevices(serviceUUIDs: [String]?) {
        discoveredPeripherals.removeAll()
        centralManager?.scanForPeripherals(withServices: serviceUUIDs?.compactMap { CBUUID(string: $0) }, options: [CBCentralManagerScanOptionAllowDuplicatesKey : true])
    }
    
    func closePeripheralConnection(_ peripheralConnection: PeripheralConnection, completion: (() -> ())!) {
        disconnectCallback = completion
        centralManager?.cancelPeripheralConnection(peripheralConnection.peripheral)
    }
    
    func connectPeripheral(_ peripheralConnection: PeripheralConnection, timeout: Int, connectionCallback: ConnectionCallback!) {
        currentPeripheralConnection = peripheralConnection
        
        connectionCallbacks[peripheralConnection.peripheral.identifier] = connectionCallback
        if peripheralConnection.peripheral.state == .connected {
            connectionCallback(peripheralConnection, nil)
        } else {
            centralManager?.connect(peripheralConnection.peripheral, options: nil)
            cancelCurrentConectionAttemptWithTimeout(timeout: timeout, peripheralConnection: peripheralConnection)
        }
    }
    
    private func cancelCurrentConectionAttemptWithTimeout(timeout: Int, peripheralConnection: PeripheralConnection) {
        DispatchQueue.main.asyncAfter(deadline: .now() + .seconds(timeout)) { [weak self] in
            if peripheralConnection.peripheral.state == .connected || self?.currentPeripheralConnection?.peripheral != peripheralConnection.peripheral {
                return
            }
            
            print("Cancel pending peripheral connection request after timeout")
            self?.centralManager!.cancelPeripheralConnection(peripheralConnection.peripheral)
            if let connectionCallback = self?.connectionCallbacks.removeValue(forKey: peripheralConnection.peripheral.identifier) {
                connectionCallback(peripheralConnection, PeripheralConnectionError.timeout)
            }
        }
    }
    
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        bluetoothConnected = false
        
        switch central.state {
        case .poweredOff:
            print("CBCentralManager state: powered Off")
        case .poweredOn:
            bluetoothConnected = true
            print("CBCentralManager state: powered On")
        case .resetting:
            print("CBCentralManager state: reserring")
        case .unauthorized:
            print("CBCentralManager state: unauthorized")
        case .unsupported:
            print("CBCentralManager state: unsupported")
        case .unknown:
            print("CBCentralManager state: unknown")
        }
        
        NotificationCenter.default.post(name: MeshNotifications.BluetoothStateUpdated, object: nil, userInfo: ["isPoweredOn" : bluetoothConnected])
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        let macAddress = searchForMacAddress(in: advertisementData)
        print("DiscoveredMeshNode: ", peripheral, "AdvertisementData: ", advertisementData, "withMacAddress: ", macAddress?.hexEncodedString() ?? "nil")
        if (discoveredPeripherals[peripheral.identifier] == nil) {
            let peripheralConnection = PeripheralConnection(peripheral, advertisementData)
            peripheralConnection.macAddress = macAddress
            peripheralConnection.rssi = RSSI
            discoveredPeripherals[peripheral.identifier] = peripheralConnection
        } else if RSSI.intValue != 127 {
            discoveredPeripherals[peripheral.identifier]?.rssi = RSSI
        }
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        print("Connect to peripheral: ", peripheral)
        if currentPeripheralConnection?.peripheral == peripheral {
            print("Connect to peripheral connection: ", currentPeripheralConnection ?? "nil")
            connectionCallbacks[peripheral.identifier]?(currentPeripheralConnection, nil)
        }
    }
    
    func centralManager(_ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral, error: Error?) {
        print("Did fail to connect peripheral: ", peripheral)
        guard currentPeripheralConnection?.peripheral == peripheral else {
            return
        }
        
        let referenceConnection = currentPeripheralConnection
        currentPeripheralConnection = nil
        
        if let connectionCallback = self.connectionCallbacks.removeValue(forKey: peripheral.identifier) {
            connectionCallback(referenceConnection, PeripheralConnectionError.didFailToConnect)
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        print("Did disconnect peripheral: ", peripheral)
        guard currentPeripheralConnection?.peripheral == peripheral else {
            return
        }
        
        let referenceConnection = currentPeripheralConnection
        currentPeripheralConnection = nil
        
        guard let connectionCallback = self.connectionCallbacks.removeValue(forKey: peripheral.identifier) else {
            return
        }
        
        guard error != nil else {
            disconnectCallback?()
            disconnectCallback = nil
            return
        }
        
        connectionCallback(referenceConnection, PeripheralConnectionError.didLoseConnection)
    }
    
    func findPeripheralConnection(for gattHandle: Int) -> PeripheralConnection? {
        return discoveredPeripherals.first { $1.gattHandle == gattHandle }?.value
    }
    
    private func searchForMacAddress(in advertisementData: [String : Any]) -> Data? {
        guard let serviceData = advertisementData["kCBAdvDataServiceData"] as? NSDictionary else {
            return nil
        }
        
        guard let data = serviceData[CBUUID(string: MeshConstants.meshAdvertimentDataKeyForMacAddress)] as? NSData else {
            return nil
        }
        
        return Data(data.subdata(with: NSMakeRange(10, 6)).reversed())
    }
}
