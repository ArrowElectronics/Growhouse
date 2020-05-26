//
//  NetworkConfigurationViewModel.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 05.06.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import BluetoothMesh
import RxSwift
import RxCocoa

class NetworkConfigurationViewModel: BaseViewModel {
    
    let subnet: SBMSubnet
    let networkName: Variable<String?>
    let canSaveName: Observable<Bool>
    
    init(_ services: ViewModelServices, subnet: SBMSubnet) {
        self.subnet = subnet
        networkName = Variable(subnet.name)
        canSaveName = networkName.asObservable().map { !($0 ?? "").trimmingCharacters(in: .whitespacesAndNewlines).isEmpty }
        
        super.init(services)
    }
    
    func changeNetworkName(to name: String) {
        subnet.name = name
        NotificationCenter.default.post(name: BluetoothMeshNotifications.NetworksUpdated, object: nil)
    }
}
