//
//  ProvisionCellViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 12/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import RxSwift
import RxCocoa

class ProvisionCellViewModel: BaseViewModel {
    
    var device: BluetoothDevice
    let name: String
    let macAddress: String
    let identifier: UUID
    let signalPower = Variable(String())
    
    private let disposeBag = DisposeBag()
    
    init(services: ViewModelServices, device: BluetoothDevice) {
        self.device = device
        name = device.name()
        macAddress = device.macAddress?.hexEncodedString() ?? "N/A"
        identifier = device.peripheral.identifier
        
        super.init(services)
        
        monitorSignalPower()
    }
    
    private func monitorSignalPower() {
        services.bluetoothManager.discoveredDevices[identifier]?.rx
            .observeWeakly(NSNumber.self, "rssi")
            .map { String(format: "\($0?.stringValue ?? "--") dBm") }
            .bind(to: signalPower)
            .disposed(by: disposeBag)
    }
}
