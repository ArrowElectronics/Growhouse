//
//  BaseViewModel.swift
//  ble_mesh-app
//
//  Created by Lukasz Rzepka on 09.04.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import RxSwift
import RxCocoa

class BaseViewModel {
    
    let services: ViewModelServices
    let isConnected = Variable(false)
    private let disposeBag = DisposeBag()
    
    convenience init() {
        self.init(ViewModelServices.sharedInstance)
    }
    
    init(_ services: ViewModelServices) {
        self.services = services
        self.services.bluetoothMeshService.connected.asObservable().bind(to: isConnected).disposed(by: disposeBag)
    }
}
