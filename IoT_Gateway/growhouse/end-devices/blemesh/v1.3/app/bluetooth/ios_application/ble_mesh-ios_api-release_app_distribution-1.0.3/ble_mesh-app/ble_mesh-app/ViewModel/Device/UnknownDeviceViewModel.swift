//
//  UnknownDeviceViewModel.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 15.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit

class UnknownDeviceViewModel: BaseViewModel {
    
    var isConnected = false
    var name: String?
    var index: String?
    
    init(_ services: ViewModelServices, name: String, index: String) {
        self.name = name
        self.index = index
        super.init(services)
    }
}
