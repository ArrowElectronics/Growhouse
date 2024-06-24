//
//  NetworkCellViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 12/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import Foundation
import BluetoothMesh

class NetworkCellViewModel {
    let network : SBMMeshNetwork
    let name : String
    let info : String
    
    init(network: SBMMeshNetwork, deviceCount: Int, groupCount: Int) {
        self.network = network
        self.name = network.name
        let devices = String.localizedStringWithFormat(NSLocalizedString("%d devices", comment: ""), deviceCount)
        let groups = String.localizedStringWithFormat(NSLocalizedString("%d groups", comment: ""), groupCount)
        self.info = String(format: "%@    %@", devices, groups)
    }
}
