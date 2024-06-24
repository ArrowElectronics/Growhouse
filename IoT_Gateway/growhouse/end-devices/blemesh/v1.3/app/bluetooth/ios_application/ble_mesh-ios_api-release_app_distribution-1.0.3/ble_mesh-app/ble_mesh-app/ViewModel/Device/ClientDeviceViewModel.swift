//
//  ClientDeviceViewModel.swift
//  ble_mesh-app
//
//  Created by Lukasz Rzepka on 02/07/2019.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import BluetoothMesh
import RxCocoa
import RxSwift

class ClientDeviceViewModel: BaseViewModel {
    
    var isConnected = false
    var device: SBMMeshNode!
    var index: String?
    let observableOn: Variable<SBMDeviceFunctionality?>
    
    private let disposeBag = DisposeBag()
    
    init(_ services: ViewModelServices, device: SBMMeshNode, index: String) {
        self.device = device
        self.index = index
        self.observableOn = Variable(device.configuration?.activeFunctionality)
        
        super.init(services)
    }
}
