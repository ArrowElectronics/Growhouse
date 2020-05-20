//
//  ClientDeviceViewModel.swift
//  ble_mesh-app
//
//  Created by Lukasz Rzepka on 02/07/2019.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import BluetoothMesh

class ClientDeviceViewModel: BaseViewModel {
    
    let device: SBMNode
    let index: String
    
    init(_ services: ViewModelServices, device: SBMNode, index: String) {
        self.device = device
        self.index = index
        
        super.init(services)
    }
    
    private let functionalityImages: [SBMModelIdentifier : UIImage] = [.genericOnOffClient : #imageLiteral(resourceName: "toggle_ON"),
                                                                       .genericPowerOnOffClient : #imageLiteral(resourceName: "toggle_ON"),
                                                                       .lightLightnessClient : #imageLiteral(resourceName: "toggle_level"),
                                                                       .genericLevelClient : #imageLiteral(resourceName: "toggle_level"),
                                                                       .lightCTLClient : #imageLiteral(resourceName: "toggle_level")]
    
    var functionalityImage: UIImage? {
        guard let functionality = services.bluetoothMeshNodeFunctionality.activeFunctionality(for: device), let modelIdentifier = SBMModelIdentifier(rawValue: functionality) else {
            return nil
        }
        
        return functionalityImages[modelIdentifier]
    }
}
