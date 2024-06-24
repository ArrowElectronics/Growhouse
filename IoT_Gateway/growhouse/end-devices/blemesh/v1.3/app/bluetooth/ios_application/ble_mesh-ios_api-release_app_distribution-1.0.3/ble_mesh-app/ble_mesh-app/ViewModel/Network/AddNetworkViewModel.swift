//
//  AddNetworkViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 12/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import RxSwift
import RxCocoa
import SVProgressHUD

class AddNetworkViewModel: BaseViewModel {
    let name = Variable<String?>(nil)
    let disposeBag = DisposeBag()
    let canAddNetwork : Observable<Bool>

    override init(_ services: ViewModelServices) {
        canAddNetwork = name.asObservable().map { !($0 ?? "").trimmingCharacters(in: .whitespacesAndNewlines).isEmpty }
        
        super.init(services)
    }
    
    func addNetwork() {
        do {
            _ = try services.networkManager.createNetwork(name: name.value ?? "unknown")
        } catch {
            SVProgressHUD.showError(withStatus: "Error creating network!")
            SVProgressHUD.dismiss(withDelay: 2)
        }
    }
}
