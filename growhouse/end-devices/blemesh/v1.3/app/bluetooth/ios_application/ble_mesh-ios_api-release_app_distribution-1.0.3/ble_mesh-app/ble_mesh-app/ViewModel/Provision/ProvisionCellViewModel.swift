//
//  ProvisionCellViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 12/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import RxSwift
import RxCocoa

class ProvisionCellViewModel: BaseViewModel {
    var model: PeripheralConnection
    var name: String
    var macAddress: String
    var signalPower: Variable<String>
    
    private let disposeBag = DisposeBag()
    
    init(services : ViewModelServices, peripheral : PeripheralConnection) {
        model = peripheral
        name = peripheral.name ?? "Unknown"
        macAddress = peripheral.macAddress?.hexEncodedString() ?? "N/A"
        signalPower = Variable(String(format: "\(peripheral.rssi?.stringValue ?? "--") dBm"))
        
        super.init(services)
        
        services.peripheralDiscovery.discoveredPeripherals[peripheral.peripheral.identifier]!.rx
            .observeWeakly(NSNumber.self, "rssi")
            .map { String(format: "\($0?.stringValue ?? "--") dBm") }
            .bind(to: signalPower)
            .disposed(by: disposeBag)
    }
}
