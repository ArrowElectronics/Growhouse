//
//  BluetoothDevice.swift
//  ble_mesh-app
//
//  Created by Piotr Sarna on 11/12/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import BluetoothMesh
import CoreBluetooth
import RxSwift
import RxCocoa
import CocoaLumberjack

@objcMembers class BluetoothDevice: NSObject, CBPeripheralDelegate, SBMConnectableDevice {
    
    var delegate: SBMConnectableDeviceDelegate?
    var macAddress: Data?
    var deviceUuid: Data?
    var connected = BehaviorRelay(value: false)
    dynamic var rssi: NSNumber?
    let peripheral: CBPeripheral
    let advertisement: [String : Any]
    
    private var subscribeCharacteristicCallbacks = [String : SBMConnectableDeviceOperationCallback]()
    private var unsubscribeCharacteristicCallbacks = [String : SBMConnectableDeviceOperationCallback]()
    private var discoverServicesCallbacks = [String : (Bool) -> (Void)]()
    private var discoverCharacteristicsCallbacks = [String : (Bool) -> (Void)]()
    private let manager: BluetoothManager
    private let disposeBag = DisposeBag()
    
    init(withManager manager: BluetoothManager, peripheral: CBPeripheral, advertisement: [String: Any]) {
        self.manager = manager
        self.peripheral = peripheral
        self.advertisement = advertisement
        super.init()
        self.macAddress = macAddress(from: advertisement)
        self.deviceUuid = uuid(from: advertisement)
        self.peripheral.delegate = self
        self.peripheral.rx.observe(CBPeripheralState.self, "state").map { $0 == .connected }.bind(to: connected).disposed(by: disposeBag)
    }
    
    // MARK: - SBMConnectableDevice
    
    func uuid() -> Data {
        return deviceUuid ?? Data()
    }
    
    func name() -> String {
        return peripheral.name ?? "unknown"
    }
    
    func advertisementData() -> [AnyHashable : Any] {
        return advertisement
    }
    
    func isConnected() -> Bool {
        return connected.value
    }
    
    func connect(_ completion: @escaping SBMConnectableDeviceConnectionCallback) {
        manager.connect(device: self, timeout: 10, completion: completion)
    }
    
    func disconnect(_ completion: @escaping SBMConnectableDeviceConnectionCallback) {
        if connected.value {
            manager.disconnect(device: self, completion: completion)
        } else {
            completion(self, true)
        }
    }
    
    func hasService(_ service: Data, completion: @escaping SBMConnectableDeviceHasServiceCallback) {
        discover(service: CBUUID(data: service).uuidString) {
            completion(self, service, $0)
        }
    }
    
    func mtu(forService serviceUuid: Data, characteristic: Data, completion: @escaping SBMConnectableDeviceMTUCallback) {
        completion(self, serviceUuid, characteristic, UInt(peripheral.maximumWriteValueLength(for: .withoutResponse)));
    }
    
    func write(_ data: Data, service: String, characteristic: String, completion: @escaping SBMConnectableDeviceOperationCallback) {
        if let cbCharacteristic = self.characteristic(with: characteristic, service: service) {
            Logger.log(message: "Peripheral \(peripheral) did write value to characteristic: \(characteristic)")
            peripheral.writeValue(data, for: cbCharacteristic, type: .withoutResponse)
            completion(self, cbCharacteristic.service.uuid.data, cbCharacteristic.uuid.data, true)
        } else {
            completion(self, CBUUID(string: service).data, CBUUID(string: characteristic).data, false)
        }
    }
    
    func subscribe(forService service: String, characteristic: String, completion: @escaping SBMConnectableDeviceOperationCallback) {
        discoverCharacteristics(for: service) {
            if let cbCharacteristic = self.characteristic(with: characteristic, service: service), $0 {
                self.subscribeCharacteristicCallbacks[cbCharacteristic.uuid.uuidString] = completion
                self.peripheral.setNotifyValue(true, for: cbCharacteristic)
            } else {
                completion(self, CBUUID(string: service).data, CBUUID(string: characteristic).data, false)
            }
        }
    }
    
    // MARK: - CBPeripheralDelegate
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        Logger.log(message: "Peripheral \(peripheral) did update value for characteristic: \(characteristic)")
        delegate?.didUpdate(characteristic.value, forDevice: self, service: characteristic.service.uuid.data, characteristic: characteristic.uuid.data)
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateNotificationStateFor characteristic: CBCharacteristic, error: Error?) {
        Logger.log(message: "Peripheral \(peripheral) did update notification state for characteristic: \(characteristic)")
        if let subCallback = subscribeCharacteristicCallbacks.removeValue(forKey: characteristic.uuid.uuidString) {
            subCallback(self, characteristic.service.uuid.data, characteristic.uuid.data, error == nil && characteristic.isNotifying)
        } else if let unsubCallback = unsubscribeCharacteristicCallbacks.removeValue(forKey: characteristic.uuid.uuidString) {
            unsubCallback(self, characteristic.service.uuid.data, characteristic.uuid.data, error == nil && !characteristic.isNotifying)
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        Logger.log(message: "Peripheral \(peripheral) did discover services")
        discoverServicesCallbacks.removeValue(forKey: peripheral.identifier.uuidString)?(error == nil)
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        Logger.log(message: "Peripheral \(peripheral) did discover characteristics for service \(service)")
        discoverCharacteristicsCallbacks.removeValue(forKey: peripheral.identifier.uuidString)?(true)
    }
    
    // MARK: - Private
    
    private func discoverCharacteristics(for service: String, completion: @escaping (Bool) -> (Void)) {
        discover(service: service) {
            let discoveredService = self.peripheral.services?.first { $0.uuid.uuidString == service }
            $0 && discoveredService != nil ? self.discoverCharacteristics(for: discoveredService!, completion: completion) : completion(false)
        }
    }
    
    private func discover(service: String, completion: @escaping (Bool) -> (Void)) {
        discoverServicesCallbacks[peripheral.identifier.uuidString] = completion
        peripheral.discoverServices([CBUUID(string: service)])
    }
    
    private func discoverCharacteristics(for service: CBService, completion: @escaping (Bool) -> (Void)) {
        discoverCharacteristicsCallbacks[peripheral.identifier.uuidString] = completion
        peripheral.discoverCharacteristics(nil, for: service)
    }
    
    private func characteristic(with uuid: String, service: String) -> CBCharacteristic? {
        return peripheral.services?.first { $0.uuid.uuidString == service }?.characteristics?.first { $0.uuid.uuidString == uuid }
    }
    
    private func uuid(from advertisement: [String : Any]) -> Data? {
        return provisioningServiceData(from: advertisement)?.subdata(in: 0..<16)
    }
    
    private func macAddress(from advertisement: [String : Any]) -> Data? {
        guard let data = provisioningServiceData(from: advertisement) else { return nil }
        return Data(data.subdata(in: 10..<16).reversed())
    }
    
    private func provisioningServiceData(from advertisement: [String : Any]) -> Data? {
        guard let serviceData = advertisement[CBAdvertisementDataServiceDataKey] as? NSDictionary else {
            return nil
        }
        
        return serviceData[CBUUID(string: "1827")] as? Data
    }
}
