//
//  AddGroupViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 23/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import BluetoothMesh
import RxSwift

class AddGroupViewModel : BaseViewModel {
    
    let name = Variable<String?>(nil)
    let canAddGroup: Observable<Bool>
    
    private let subnet: SBMSubnet
    
    init(_ services: ViewModelServices, subnet: SBMSubnet) {
        self.subnet = subnet
        canAddGroup = name.asObservable().map { !($0 ?? "").trimmingCharacters(in: .whitespacesAndNewlines).isEmpty }
        
        super.init(services)
    }
    
    func addGroup() {
        services.bluetoothMeshNetworkManager.createGroup(name: name.value!, subnet: subnet)
    }
}
