//
//  LevelDeviceViewModel.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 24.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import BluetoothMesh
import RxCocoa
import RxSwift

class LevelDeviceViewModel: BaseViewModel {
    
    var isConnected = false
    var index: String?
    var percentage: Variable<Float>
    var stateReceivedCallback: (Float) -> () = { _ in }
    let device: SBMMeshNode!
    let observablePercentage: Variable<Float>
    
    private var percentageWasSet = false
    private let disposeBag = DisposeBag()
    
    init(_ services: ViewModelServices, device: SBMMeshNode, index: String, percentage: Float) {
        self.device = device
        self.index = index
        self.percentage = Variable(percentage)
        self.observablePercentage = Variable(percentage)
        super.init(services)
        
        self.observablePercentage.asObservable()
            .buffer(timeSpan: 0.25, count: 0, scheduler: MainScheduler.instance)
            .map { $0.last }
            .filter { $0 != nil }
            .skip(1)
            .subscribe(onNext: { [weak self] (value) in
                self?.change(percentage: value!)
                self?.percentage.value = value!
            })
            .disposed(by: disposeBag)
        
        guard let group = services.networkManager.group(withMeshNode: device) else {
            return
        }
        
        services.bluetoothMeshService.bleMeshManager.getLevel(device, from: group) { [weak self] (level) in
            if let weakSelf = self, !weakSelf.percentageWasSet {
                weakSelf.percentage.value = Float(Int(level) + Int(Int16.max) + 1) / Float(UInt16.max)
                weakSelf.stateReceivedCallback(weakSelf.percentage.value)
            }
        }
    }
    
    private func change(percentage: Float) {
        guard let network = services.networkManager.network(withMeshNode: device) else {
            return
        }
        
        let groups = services.networkManager.getGroups(withNetwork: network)
        
        guard let configuration = device.configuration else {
            return
        }
        
        let maybeGroup = groups.first { group in configuration.applicationKeys.contains {$0.key == group.applicationKeyIndex }}
        
        guard let group = maybeGroup else {
            return
        }
        
        if !percentageWasSet {
            percentageWasSet = true
            stateReceivedCallback(percentage)
        }
        
        var status = Int(percentage * Float(UInt16.max)) - Int(Int16.max)
        if status > Int16.max {
            status = Int(Int16.max)
        }
        
        services.bluetoothMeshService.bleMeshManager.setLevel(device, in: group, status: Int16(status), transitionMS: UInt32(0.0), delayMS: UInt16(0.0), requestReply: false, isFinal: false)
    }
}
