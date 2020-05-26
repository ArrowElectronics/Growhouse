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

class LightDeviceViewModel: BaseViewModel, UpdatableViewModel {
    
    let device: SBMNode
    let index: String
    
    var changeFromNetworkLightness: Variable<ReactiveResult<Double>>
    private let toChangeByViewLightness : Variable<Double>

    private let disposeBag = DisposeBag()
    
    init(_ services: ViewModelServices, device: SBMNode, index: String, percentage: Double) {
        self.device = device
        self.index = index
        self.changeFromNetworkLightness = Variable(ReactiveResult(value: Double.nan, error: nil))
        self.toChangeByViewLightness = Variable<Double>(percentage)
        
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
        
        changeFromNetworkLightness.value = ReactiveResult(value: value, error: nil)
    }
    
    func changeLightOnOff() {
        let lightness = changeFromNetworkLightness.value
        setLightness(value: lightness.value < 0.01 ? 1.0 : 0.0)
    }
    
    func setLightness(value: Double) {
        changeFromNetworkLightness.value = ReactiveResult(value: value, error: nil)
        toChangeByViewLightness.value = value
    }
    
    func getValuesFromNetwork(completion: (() -> ())? = nil) {
        services.bluetoothMeshElementControl.getLightness(for: device) { [weak self] (lightness, error) in
            guard error == nil, let lightness = lightness else {
                self?.handleErrorFromNetwork(error: error)
                return
            }
            
            self?.changeFromNetworkLightness.value = ReactiveResult(value: Double(lightness) / Double(UInt16.max), error: nil)
        }
    }
    
    private func handleErrorFromNetwork(error: Error?) {
        let errorToSend: Error
        
        if error == nil {
            errorToSend = NSError(domain: "com.silabs.bluetoothmesh.light.notKnown", code: -1, userInfo: nil)
        } else {
            errorToSend = error!
        }
        
        changeFromNetworkLightness.value = ReactiveResult(value: Double.nan, error: errorToSend)
    }
    
    private func addObservables() {
        self.toChangeByViewLightness.asObservable()
            .buffer(timeSpan: 0.5, count: 0, scheduler: MainScheduler.instance)
            .map { $0.last }
            .filter { $0 != nil }
            .skip(1)
            .subscribe(onNext: { [weak self] (value) in
                self?.changeLightness(percentage: value!)
            })
            .disposed(by: disposeBag)
    }
    
    private func changeLightness(percentage: Double) {
        let status = Int(percentage * Double(UInt16.max))
        services.bluetoothMeshElementControl.set(lightness: status, for: device)
    }
}
