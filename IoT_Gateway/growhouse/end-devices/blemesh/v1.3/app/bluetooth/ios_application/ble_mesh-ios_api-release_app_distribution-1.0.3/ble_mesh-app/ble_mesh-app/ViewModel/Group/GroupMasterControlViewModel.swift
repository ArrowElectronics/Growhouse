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
    let observablePercentage : Variable<Double>
    
    private let delayMS = 0.0
    private let transitionMS = 0.0
    private let group: SBMMeshGroup
    private let disposeBag = DisposeBag()
    
    init(_ services: ViewModelServices, group: SBMMeshGroup, isOn: Bool, percentage: Double) {
        self.group = group
        self.isOn = isOn
        self.observablePercentage = Variable<Double>(percentage)
        super.init(services)
        
        self.observablePercentage.asObservable()
            .buffer(timeSpan: 0.25, count: 0, scheduler: MainScheduler.instance)
            .map { (values) -> Double? in
                return values.last
            }.filter { $0 != nil }
            .skip(1)
            .subscribe(onNext: { [weak self] (value) in
                self?.change(percentage: value!)
            }).disposed(by: disposeBag)
    }
    
    private func change(percentage: Double) {
        let status = UInt16(percentage * Double(UInt16.max))
        
        services.bluetoothMeshService.bleMeshManager.setDimmable(nil, in: group, status: status, transitionMS: UInt32(transitionMS), delayMS: UInt16(delayMS), requestReply: false, isFinal: false)
    }
    
    func change(on: Bool) {
        isOn = on
        services.bluetoothMeshService.bleMeshManager.setOnOff(nil, in: group, status: on ? 1 : 0, transitionMS: UInt32(transitionMS), delayMS: UInt16(delayMS), requestReply: false, isFinal: false)
    }
}
