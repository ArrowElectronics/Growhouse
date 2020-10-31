//
//  AddGroupViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 23/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import Foundation
import RxSwift
import BluetoothMesh
import SVProgressHUD

class AddGroupViewModel : BaseViewModel {
    let name = Variable<String?>(nil)
    let networkName : String
    let disposeBag = DisposeBag()
    let canAddGroup : Observable<Bool>
    
    private let network : SBMMeshNetwork
    
    init(_ services: ViewModelServices, network: SBMMeshNetwork) {
        self.network = network
        self.networkName = network.name
        canAddGroup = name.asObservable().map { !($0 ?? "").trimmingCharacters(in: .whitespacesAndNewlines).isEmpty }
        
        super.init(services)
    }
    
    func addGroup() {
        do {
        _ = try services.networkManager.createGroup(name: name.value!, network: network)
        } catch {
            SVProgressHUD.showError(withStatus: "Error creating group!")
            SVProgressHUD.dismiss(withDelay: 2)
        }
    }
}
