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
    
    let elements = Variable<[SBMElement]>([])

    init(_ services: ViewModelServices, node: SBMMeshNode) {
        super.init(services)
        
        services.bluetoothMeshService.deviceCompositionData(for: node) { [weak self] (data, error) in
            guard let dcd = data, let elements = dcd.elements else {
                return
            }
            
            self?.elements.value = elements
        }
    }
}
