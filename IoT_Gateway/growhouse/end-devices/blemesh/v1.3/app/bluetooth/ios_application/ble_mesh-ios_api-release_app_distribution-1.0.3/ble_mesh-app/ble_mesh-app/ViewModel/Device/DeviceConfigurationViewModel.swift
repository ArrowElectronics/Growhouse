//
//  DeviceConfigurationViewModel.swift
//  ble_mesh-app
//
//  Created by Lukasz Rzepka on 20.04.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import Foundation
import BluetoothMesh
import RxCocoa
import RxSwift

class DeviceConfigurationViewModel: BaseViewModel {
    let deviceConfigurationDetailViewModel: DeviceConfigurationDetailViewModel
    var isConnected = Variable(true)
    
    init(_ services: ViewModelServices, device: SBMMeshNode) {
        deviceConfigurationDetailViewModel = DeviceConfigurationDetailViewModel(services, device: device)
        super.init(services)
    }
}
