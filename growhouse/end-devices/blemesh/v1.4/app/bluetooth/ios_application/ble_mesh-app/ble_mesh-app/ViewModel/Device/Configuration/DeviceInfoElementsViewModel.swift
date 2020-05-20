//
//  DeviceInfoElementsViewModel.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 25.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import BluetoothMesh
import RxSwift

class DeviceInfoElementsViewModel: BaseViewModel {
    
    let elements = Variable([SBMElement]())

    init(_ services: ViewModelServices, elements: [SBMElement]) {
        self.elements.value = elements
        
        super.init(services)
    }
}
