//
//  LightDeviceViewModel.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 15.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import BluetoothMesh
import RxSwift
import RxCocoa

class LightDeviceViewModel: BaseViewModel {
    
    let observablePercentage : Variable<Double>
    let device: SBMMeshNode!
    var isConnected = false
    var index: String?
    var percentage: Variable<Double>
    var stateReceivedCallback: (Double) -> () = { _ in }
    
    private var percentageWasSet = false

    private let delayMS = 0.0
    private let transitionMS = 0.0

    private let disposeBag = DisposeBag()
    
    init(_ services: ViewModelServices, device: SBMMeshNode, index: String, percentage: Double) {
        self.device = device
        self.index = index
        self.percentage = Variable(percentage)
        self.observablePercentage = Variable<Double>(percentage)
        
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
        
        services.bluetoothMeshService.bleMeshManager.getDimmable(device, from: group) { [weak self] (level) in
            if let weakSelf = self, !weakSelf.percentageWasSet {
                weakSelf.percentage.value = Double(level) / Double(UInt16.max)
                weakSelf.stateReceivedCallback(weakSelf.percentage.value)
            }
        }
        
        NotificationCenter.default.addObserver(forName: MeshNotifications.GroupValueChanged, object: nil, queue: .main) { [weak self] (notification) in
            let value = notification.userInfo?["value"] as! Double
            self?.percentage.value = value
        }
    }
    
    private func change(percentage: Double) {
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
        
        let status = UInt16(percentage * Double(UInt16.max))
        
        services.bluetoothMeshService.bleMeshManager.setDimmable(device, in: group, status: status, transitionMS: UInt32(transitionMS), delayMS: UInt16(delayMS), requestReply: false, isFinal: false)
    }
}
