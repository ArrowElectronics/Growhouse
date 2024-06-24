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
    
    let network: SBMMeshNetwork
    let networkName: Variable<String?>
    let canSaveName: Observable<Bool>
    
    init(_ services: ViewModelServices, network: SBMMeshNetwork) {
        self.network = network
        self.networkName = Variable<String?>(network.name)
        canSaveName = self.networkName.asObservable().map { !($0 ?? "").trimmingCharacters(in: .whitespacesAndNewlines).isEmpty }
        
        super.init(services)
    }
    
    func changeNetworkName(to name: String) {
        services.bluetoothMeshService.bleMeshManager.change(network, name: name)
        NotificationCenter.default.post(name: MeshNotifications.NetworksUpdated, object: nil)
    }
}
