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

class SwitchDeviceViewModel: BaseViewModel, UpdatableViewModel {

    let device: SBMNode
    let index: String
    
    let changeFromNetworkSwitchState: Variable<ReactiveResult<Bool>>
    private let toChangeByViewSwitchState: Variable<Bool>
    
    private let disposeBag = DisposeBag()
    
    init(_ services: ViewModelServices, device: SBMNode, index: String, isOn: Bool) {
        self.device = device
        self.index = index
        self.changeFromNetworkSwitchState = Variable(ReactiveResult(value: false, error: nil))
        self.toChangeByViewSwitchState = Variable(isOn)
        
        super.init(services)
        
        self.addObservables()
        
        NotificationCenter.default.addObserver(self,
                                               selector: #selector(valueMasterGroupDidChange(notification:)),
                                               name: BluetoothMeshNotifications.GroupValueChanged, object: nil)
    }
    
    @objc private func valueMasterGroupDidChange(notification: Notification) {
        guard let value = notification.userInfo?["value"] as? Double else {
            return
        }
        
        changeFromNetworkSwitchState.value = ReactiveResult(value: value > 0.0, error: nil)
    }
    
    func changeSwitchState() {
        let newValue = changeFromNetworkSwitchState.value.value ? false : true
        changeFromNetworkSwitchState.value = ReactiveResult(value: newValue, error: nil)
        toChangeByViewSwitchState.value = newValue
    }
    
    func getValuesFromNetwork(completion: (() -> ())? = nil) {
        services.bluetoothMeshElementControl.getOnOff(for: device) { [weak self] (isOn, error) in
            guard error == nil, let isOn = isOn else {
                self?.handleErrorFromNetwork(error: error)
                return
            }
            
            self?.changeFromNetworkSwitchState.value = ReactiveResult(value:isOn, error: nil)
        }
    }
    
    private func handleErrorFromNetwork(error: Error?) {
        let errorToSend: Error
        
        if error == nil {
            errorToSend = NSError(domain: "com.silabs.bluetoothmesh.isOn.notKnown", code: -1, userInfo: nil)
        } else {
            errorToSend = error!
        }
        
        changeFromNetworkSwitchState.value = ReactiveResult(value: false, error: errorToSend)
    }
    
    private func addObservables() {
        self.toChangeByViewSwitchState.asObservable().skip(1).subscribe(onNext: { [weak self] (on) in
            self?.changeSwitch(on: on)
        }).disposed(by: disposeBag)
    }
    
    private func changeSwitch(on: Bool) {
        services.bluetoothMeshElementControl.set(onOff: on, for: device)
    }
}
