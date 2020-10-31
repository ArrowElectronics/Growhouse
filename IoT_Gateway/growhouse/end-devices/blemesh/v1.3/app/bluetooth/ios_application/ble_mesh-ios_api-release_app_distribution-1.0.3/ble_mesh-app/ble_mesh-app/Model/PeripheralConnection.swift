//
//  BluetoothPeripheral.swift
//  ble_mesh-app
//
//  Created by Lukasz Rzepka on 09.04.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import Foundation
import CoreBluetooth
import RxCocoa
import RxSwift

typealias DiscoverServicesCallback = (_ withSuccess: Bool) -> ()
typealias DiscoverCharacteristicsCallback = (_ serviceUUID: String, _ withSuccess: Bool) -> ()
typealias SubscribeCharacterisitcCallback = (_ withSuccess: Bool) -> ()

protocol PeripheralConnectionDelegate: class {
    func didUpdateValueFor(_ peripheralConnection: PeripheralConnection, _ characteristic: CBCharacteristic)
}

@objcMembers class PeripheralConnection: NSObject, CBPeripheralDelegate {
    dynamic var characteristics = [CBUUID: CBCharacteristic]()
    var discoverServicesCallback: DiscoverServicesCallback?
    var discoverCharacteristicsCallbacks = [String: DiscoverCharacteristicsCallback]()
    var subscribeCharacteristicCallbacks = [String: SubscribeCharacterisitcCallback]()
    
    let peripheral: CBPeripheral
    var isConnected: BehaviorRelay<Bool> = BehaviorRelay(value: Bool())
    var macAddress: Data? = nil
    var meshDeviceUUID: Data? = nil
    let advertisementData: [String : Any]
    let name: String?
    var gattHandle: Int
    dynamic var rssi : NSNumber?
    weak var delegate: PeripheralConnectionDelegate?
    private let disposeBag = DisposeBag()
    
    init(_ peripheral: CBPeripheral,_ advertisementData: [String : Any]) {
        self.peripheral = peripheral
        self.advertisementData = advertisementData
        self.name = peripheral.name
        self.gattHandle = Int(arc4random_uniform(UInt32(Int16.max)))
        super.init()
        
        let connectionSequence = self.peripheral.rx.observe(CBPeripheralState.self, "state")
        connectionSequence.subscribe(onNext: { [weak self] (peripheralState) in
            guard let state = peripheralState else {
                return
            }
            
            switch state {
            case .disconnected:
                self?.isConnected.accept(false)
            case .connecting:
                self?.isConnected.accept(false)
            case .connected:
                self?.isConnected.accept(true)
            case .disconnecting:
                self?.isConnected.accept(false)
            }
        }).disposed(by: disposeBag)
        
        searchForUUID()
    }
    
    func subscribeCharacteristics(_ characteristicUUID: String, forServiceUUID: String, withCallback: SubscribeCharacterisitcCallback!) {
        if let readCharacteristic: CBCharacteristic = characteristicForUUID(characteristicUUID, serviceUUID: forServiceUUID) {
            if readCharacteristic.isNotifying == false {
                subscribeCharacteristicCallbacks[characteristicUUID] = withCallback
                subscribeToCharacteristic(readCharacteristic)
            } else {
                withCallback(true)
            }
        } else {
            withCallback(false)
        }
    }
    
    func discoverCharacteristicsForService(_ uuid: String, withCallback: DiscoverCharacteristicsCallback!) {
        discoverServicesForUUID(uuid) { [weak self] success in
            if success {
                if let service = self?.serviceForUUID(uuid) {
                    self?.discoverCharacteristicsCallbacks[uuid] = withCallback
                    print("Discover characteristics for service: ", service)
                    self?.peripheral.discoverCharacteristics(nil, for: service)
                } else {
                    withCallback(uuid, false)
                }
            } else {
                withCallback(uuid, false)
            }
        }
    }
    
    private func discoverServicesForUUID(_ uuidString: String, _ callback: DiscoverServicesCallback!) {
        discoverServicesCallback = callback
        peripheral.discoverServices([CBUUID.init(string: uuidString)])
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        for characteristic in service.characteristics! {
            print("Found characteristic: ", characteristic, "mtu: ", peripheral.maximumWriteValueLength(for: .withoutResponse))
            characteristics[characteristic.uuid] = characteristic
        }
        
        if let discoverCallback = discoverCharacteristicsCallbacks.removeValue(forKey: service.uuid.uuidString) {
            discoverCallback(service.uuid.uuidString, true)
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        if let callback = discoverServicesCallback {
            callback(true)
            discoverServicesCallback = nil
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateNotificationStateFor characteristic: CBCharacteristic, error: Error?) {
        print("Update notification from characteristic: ", characteristic)
        if let callback = subscribeCharacteristicCallbacks.removeValue(forKey: characteristic.uuid.uuidString) {
            callback(true)
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        delegate?.didUpdateValueFor(self, characteristic)
    }
    
    func characteristicForUUID(_ uuid: String, serviceUUID: String) -> (CBCharacteristic?) {
        guard let service = serviceForUUID(serviceUUID) else {
            return nil
        }
        
        let characteristic = service.characteristics?.first(where: {$0.uuid.uuidString == uuid})
        return characteristic
    }
    
    func serviceForUUID(_ uuid: String) -> (CBService?) {
        return peripheral.services?.first(where: {$0.uuid.uuidString == uuid})
    }
    
    func subscribeToCharacteristic(_ characteristic: CBCharacteristic) {
        peripheral.setNotifyValue(true, for: characteristic)
    }
    
    private func searchForUUID() {
        if let data = advertisementData["kCBAdvDataServiceData"] as? NSDictionary{
            if let newData = data[CBUUID(string: MeshConstants.meshAdvertimentDataKeyForDeviceUUID)] as? NSData{
                meshDeviceUUID = newData.subdata(with: NSMakeRange(0, 16))
            }
        }
    }
}
