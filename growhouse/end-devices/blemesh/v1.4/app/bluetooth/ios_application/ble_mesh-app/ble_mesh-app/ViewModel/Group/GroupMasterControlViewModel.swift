//
//  GroupMasterControlViewModel.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 15.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import BluetoothMesh
import RxSwift
import RxCocoa

class GroupMasterControlViewModel: BaseViewModel {
    
    var isOn: Bool
    let observablePercentage: Variable<Double>
    
    private let group: SBMGroup
    private let disposeBag = DisposeBag()
    
    init(_ services: ViewModelServices, group: SBMGroup, isOn: Bool, percentage: Double) {
        self.group = group
        self.isOn = isOn
        observablePercentage = Variable(percentage)
        super.init(services)
        
        observablePercentage.asObservable()
            .buffer(timeSpan: 0.25, count: 0, scheduler: MainScheduler.instance)
            .map { $0.last }
            .filter { $0 != nil }
            .skip(1)
            .subscribe(onNext: { [weak self] (value) in
                self?.change(percentage: value!) })
            .disposed(by: disposeBag)
    }
    
    private func change(percentage: Double) {
        services.bluetoothMeshGroupControl.set(lightness: Int(percentage * Double(UInt16.max)), for: group)
    }
    
    func change(on: Bool) {
        isOn = on
        services.bluetoothMeshGroupControl.set(onOff: on, for: group)
    }
}
