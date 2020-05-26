//
//  SwitchDeviceViewModel.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 15.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import BluetoothMesh
import RxCocoa
import RxSwift

class SwitchDeviceViewModel: BaseViewModel {
    
    var isConnected = false
    var device: SBMMeshNode!
    var index: String?
    var isOn: Bool
    let observableOn: Variable<Bool>
    var stateReceivedCallback: (Bool) -> () = { _ in }
    
    private var onWasSet = false
    
    private let delayMS = 150.0
    private let transitionMS = 0.0
    private let disposeBag = DisposeBag()
    
    init(_ services: ViewModelServices, device: SBMMeshNode, index: String, isOn: Bool) {
        self.device = device
        self.index = index
        self.isOn = isOn
        self.observableOn = Variable(isOn)
        
        super.init(services)
        
        self.observableOn.asObservable().skip(1).subscribe(onNext: { [weak self] (on) in
            self?.change(on: on)
        }).disposed(by: disposeBag)
        
        guard let group = services.networkManager.group(withMeshNode: device) else {
            return
        }
        
        services.bluetoothMeshService.bleMeshManager.getOnOff(device, from: group) { [weak self] (on) in
            if let weakSelf = self, !weakSelf.onWasSet {
                weakSelf.isOn = on > 0 ? true : false
                weakSelf.stateReceivedCallback(weakSelf.isOn)
            }
        }
        
        NotificationCenter.default.addObserver(forName: MeshNotifications.GroupValueChanged, object: nil, queue: .main) { [weak self] (notification) in
            let value = notification.userInfo?["value"] as! Double
            self?.isOn = value > 0.0
        }
    }
    
    private func change(on: Bool) {
        guard let network: SBMMeshNetwork = services.networkManager.network(withMeshNode: device) else {
            return
        }
        
        let groups = services.networkManager.getGroups(withNetwork: network)
        
        guard let configuration = device.configuration else {
            return
        }
        
        let group = groups.first { group in configuration.applicationKeys.contains { $0.key == Int(group.applicationKeyIndex)}}
        
        if !onWasSet {
            onWasSet = true
            stateReceivedCallback(on)
        }
        
        if group != nil {
            services.bluetoothMeshService.bleMeshManager.setOnOff(device, in: group!, status: on ? 1 : 0, transitionMS: UInt32(transitionMS), delayMS: UInt16(delayMS), requestReply: false, isFinal: false)
        }
    }
}
