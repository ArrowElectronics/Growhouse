//
//  BluetoothManager.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 11/12/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import CoreBluetooth
import BluetoothMesh
import RxSwift
import CocoaLumberjack

@objcMembers class BluetoothManager: NSObject, CBCentralManagerDelegate {
    
    let bluetoothConnected = Variable(false)
    let currentDevice: Variable<BluetoothDevice?> = Variable(nil)
    dynamic var discoveredDevices = [UUID: BluetoothDevice]()
    private var centralManager: CBCentralManager!
    private var connectionCallbacks = [UUID: SBMConnectableDeviceConnectionCallback]()
    private var disconnectionCallbacks = [UUID: SBMConnectableDeviceConnectionCallback]()
    private let bluetoothMeshService: BluetoothMeshService
    
    init(bluetoothMeshService: BluetoothMeshService) {
        self.bluetoothMeshService = bluetoothMeshService
        super.init()
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }
    
    func connect(device: BluetoothDevice, timeout: Int, completion: @escaping SBMConnectableDeviceConnectionCallback) {
        Logger.log(message: "Connect peripheral: \(device.peripheral)")
        currentDevice.value = device
        if device.peripheral.state == .connected {
            Logger.log(message: "Peripheral already connected")
            completion(device, true)
        } else {
            Logger.log(message: "Connecting")
            connectionCallbacks[device.peripheral.identifier] = completion
            centralManager.connect(device.peripheral)
            setup(timeout: timeout, for: device)
        }
    }
    
    func disconnect(device: BluetoothDevice, completion: @escaping SBMConnectableDeviceConnectionCallback) {
        Logger.log(message: "Disconnect peripheral: \(device.peripheral)")
        disconnectionCallbacks[device.peripheral.identifier] = completion
        centralManager.cancelPeripheralConnection(device.peripheral)
    }
    
    func startDiscovering(services: [String]) {
        Logger.log(message: "Start discovering services: \(services)")
        discoveredDevices.removeAll()
        centralManager.scanForPeripherals(withServices: services.map { CBUUID(string: $0) }, options: [CBCentralManagerScanOptionAllowDuplicatesKey : true])
    }
    
    func stopDiscovering() {
        Logger.log(message: "Stop discovering services")
        centralManager.stopScan()
    }
    
    // MARK: - CBCentralManagerDelegate
    
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        bluetoothConnected.value = false
        
        switch central.state {
        case .poweredOff:
            Logger.log(message: "Did update state: powered off")
        case .poweredOn:
            Logger.log(message: "Did update state: powered on")
            bluetoothConnected.value = true
        case .resetting:
            Logger.log(message: "Did update state: resetting")
        case .unauthorized:
            Logger.log(message: "Did update state: unauthorized")
        case .unsupported:
            Logger.log(message: "Did update state: unsupported")
        case .unknown:
            Logger.log(message: "Did update state: unknown")
        }
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        Logger.log(message: "Did connect to peripheral: \(peripheral)")
        if let device = currentDevice.value, device.peripheral == peripheral {
            connectionCallbacks.removeValue(forKey: peripheral.identifier)?(device, true)
        }
    }
    
    func centralManager(_ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral, error: Error?) {
        Logger.log(message: "Did fail to connect to peripheral: \(peripheral)")
        if let device = currentDevice.value, device.peripheral == peripheral {
            currentDevice.value = nil
            connectionCallbacks.removeValue(forKey: peripheral.identifier)?(device, false)
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        Logger.log(message: "Did disconnect peripheral: \(peripheral), with error: \(error?.localizedDescription ?? "none")")
        if let device = currentDevice.value, device.peripheral == peripheral {
            currentDevice.value = nil
            if let callback = disconnectionCallbacks.removeValue(forKey: peripheral.identifier) {
                callback(device, true)
            } else {
                bluetoothMeshService.disconnect()
            }
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        Logger.log(message: "Did discover peripheral: \(peripheral)")
        if discoveredDevices[peripheral.identifier] == nil {
            let bluetoothDevice = BluetoothDevice(withManager: self, peripheral: peripheral, advertisement: advertisementData)
            bluetoothDevice.rssi = RSSI
            discoveredDevices[peripheral.identifier] = bluetoothDevice
        } else if RSSI.intValue != 127 {
            discoveredDevices[peripheral.identifier]?.rssi = RSSI
        }
    }
    
    // MARK: - Private
    
    private func setup(timeout: Int, for device: BluetoothDevice) {
        DispatchQueue.main.asyncAfter(deadline: .now() + .seconds(timeout)) { [weak self] in
            guard device.peripheral.state != .connected && device == self?.currentDevice.value else { return }
            Logger.log(message: "Cancel connection after timeout for peripheral: \(device.peripheral)")
            self?.centralManager.cancelPeripheralConnection(device.peripheral)
            self?.connectionCallbacks.removeValue(forKey: device.peripheral.identifier)?(device, false)
        }
    }
}
