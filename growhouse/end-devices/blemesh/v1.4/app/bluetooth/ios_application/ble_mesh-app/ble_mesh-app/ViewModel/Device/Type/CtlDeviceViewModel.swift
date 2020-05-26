//
//  CtlDeviceViewModel.swift
//  ble_mesh-app
//
//  Created by Lukasz on 25.07.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import BluetoothMesh
import RxSwift
import RxCocoa

class CtlDeviceViewModel: BaseViewModel, UpdatableViewModel {
    
    let device: SBMNode
    let index: String
    
    let changeFromNetworkLightnessPercentage: Variable<ReactiveResult<Double>>
    let changeFromNetworkTemperature : Variable<ReactiveResult<Double>>
    let changeFromNetworkDeltaUv : Variable<ReactiveResult<Double>>
    
    private let toChangeByViewLightnessPercentage : Variable<Double>
    private let toChangeByViewTemperaturePercentage : Variable<Double>
    private let toChangeByViewDeltaUvPercentage : Variable<Double>
    
    private let maxTemperature = 20000.0
    private let minTemperature = 800.0
    
    private let disposeBag = DisposeBag()
    
    init(_ services: ViewModelServices, device: SBMNode, index: String, lightnessPercentage: Double, temperaturePercentage: Double, deltaUvPercentage: Double) {
        self.device = device
        self.index = index

        self.changeFromNetworkLightnessPercentage = Variable(ReactiveResult(value: Double.nan, error: nil))
        self.changeFromNetworkTemperature = Variable(ReactiveResult(value: Double.nan, error: nil))
        self.changeFromNetworkDeltaUv = Variable(ReactiveResult(value: Double.nan, error: nil))
        
        self.toChangeByViewLightnessPercentage = Variable<Double>(lightnessPercentage)
        self.toChangeByViewTemperaturePercentage = Variable<Double>(temperaturePercentage)
        self.toChangeByViewDeltaUvPercentage = Variable<Double>(deltaUvPercentage)
        
        super.init(services)
        
        self.addObservables()
        
        NotificationCenter.default.addObserver(self,
                                               selector: #selector(valueMasterGroupDidChange(notification:)),
                                               name: BluetoothMeshNotifications.GroupValueChanged,
                                               object: nil)
    }
    
    @objc private func valueMasterGroupDidChange(notification: Notification) {
        guard let value = notification.userInfo?["value"] as? Double else {
            return
        }
        
        changeFromNetworkLightnessPercentage.value = ReactiveResult(value: value, error: nil)
    }
    
    func changeLightOnOff() {
        let lightness = changeFromNetworkLightnessPercentage.value
        setLightness(value: lightness.value < 0.01 ? 1.0 : 0.0)
    }
    
    func setLightness(value: Double) {
        changeFromNetworkLightnessPercentage.value = ReactiveResult(value: value, error: nil)
        toChangeByViewLightnessPercentage.value = value
    }
    
    func setTemperature(value: Double) {
        let temperature = value * (maxTemperature - minTemperature) + minTemperature
        changeFromNetworkTemperature.value = ReactiveResult(value: temperature, error: nil)
        toChangeByViewTemperaturePercentage.value = value
    }
    
    func setDelataUV(value: Double) {
        let deltaUV = (2.0 * value) - 1
        changeFromNetworkDeltaUv.value = ReactiveResult(value: deltaUV, error: nil)
        toChangeByViewDeltaUvPercentage.value = value
    }
    
    func getValuesFromNetwork(completion: (() -> ())? = nil) {
        let dispatchGroup = DispatchGroup()
        
        dispatchGroup.enter()
        getColorTemperature() {
            dispatchGroup.leave()
        }
        dispatchGroup.enter()
        getDeltaUV() {
            dispatchGroup.leave()
        }
        
        dispatchGroup.notify(queue: .main) {
            completion?()
        }
    }
    
    private func getColorTemperature(completion: (() -> ())? = nil) {
        services.bluetoothMeshElementControl.getColorTemperature(for: device) { [weak self] (lightness, temperature, error) in
            guard error == nil, let lightness = lightness, let temperature = temperature else {
                self?.handleTemperatureErrorFromNetwork(error: error)
                completion?()
                return
            }
            
            self?.changeFromNetworkLightnessPercentage.value = ReactiveResult(value: Double(lightness) / Double(UInt16.max), error: nil)
            self?.changeFromNetworkTemperature.value = ReactiveResult(value: Double(temperature), error: nil)
            completion?()
        }
    }
    
    private func getDeltaUV(completion: (() -> ())? = nil) {
        services.bluetoothMeshElementControl.getColorDeltaUv(for: device) { [weak self]  (temperature, deltaUv, error) in
            guard error == nil, let deltaUv = deltaUv else {
                self?.handleDeltaUVErrorFromNetwork(error: error)
                completion?()
                return
            }
            
            let calculatedDeltaUV = (Double(deltaUv) + Double(Int16.max) + 1) / Double(UInt16.max) * 2 - 1
            self?.changeFromNetworkDeltaUv.value = ReactiveResult(value: calculatedDeltaUV, error: nil)
            completion?()
        }
    }
    
    private func handleTemperatureErrorFromNetwork(error: Error?) {
        let errorToSend: Error
        
        if error == nil {
            errorToSend = NSError(domain: "com.silabs.bluetoothmesh.colorTemperature.notKnown", code: -1, userInfo: nil)
        } else {
            errorToSend = error!
        }
  
        changeFromNetworkLightnessPercentage.value = ReactiveResult(value: Double.nan, error: errorToSend)
        changeFromNetworkTemperature.value = ReactiveResult(value: Double.nan, error: errorToSend)
    }
    
    private func handleDeltaUVErrorFromNetwork(error: Error?) {
        let errorToSend: Error
        
        if error == nil {
            errorToSend = NSError(domain: "com.silabs.bluetoothmesh.deltaUV.notKnown", code: -1, userInfo: nil)
        } else {
            errorToSend = error!
        }
        
        changeFromNetworkDeltaUv.value = ReactiveResult(value: Double.nan, error: errorToSend)
    }
    
    private func addObservables() {
        self.toChangeByViewLightnessPercentage.asObservable()
            .buffer(timeSpan: 0.5, count: 0, scheduler: MainScheduler.instance)
            .map { $0.last }
            .filter { $0 != nil }
            .skip(1)
            .subscribe(onNext: { [weak self] (percentage) in
                self?.changeLightness(percentage: percentage!)
            })
            .disposed(by: disposeBag)
        
        self.toChangeByViewTemperaturePercentage.asObservable()
            .buffer(timeSpan: 0.5, count: 0, scheduler: MainScheduler.instance)
            .map { $0.last }
            .filter { $0 != nil }
            .skip(1)
            .subscribe(onNext: { [weak self] (percentage) in
                self?.changeTemperature(percentage: percentage!)
            })
            .disposed(by: disposeBag)
        
        
        self.toChangeByViewDeltaUvPercentage.asObservable()
            .buffer(timeSpan: 0.5, count: 0, scheduler: MainScheduler.instance)
            .map { $0.last }
            .filter { $0 != nil }
            .skip(1)
            .subscribe(onNext: { [weak self] (percentage) in
                self?.changeDeltaUv(percentage!)
            })
            .disposed(by: disposeBag)
    }
    
    private func changeLightness(percentage: Double) {
        let lightness = UInt16(percentage * Double(UInt16.max))
        let temperature = changeFromNetworkTemperature.value.value.isNaN ? minTemperature : changeFromNetworkTemperature.value.value
        changeFromNetworkTemperature.value = ReactiveResult(value: temperature, error: nil)
        let deltaUV = changeFromNetworkDeltaUv.value.value.isNaN ? -1.0 : changeFromNetworkDeltaUv.value.value
        changeFromNetworkDeltaUv.value = ReactiveResult(value: deltaUV, error: nil)
        
        let calculatedDeltaUV = Int16((deltaUV + 1.0) / 2.0 * Double(UInt16.max) - (Double(Int16.max) + 1))
        let calculatedTemperature = UInt16(temperature)
        setCTL(lightness: lightness, temperature: calculatedTemperature, deltaUV: calculatedDeltaUV)
    }
    
    private func changeTemperature(percentage: Double) {
        let temperature = UInt16(percentage * Double(maxTemperature - minTemperature)) + 800
        let lightness = changeFromNetworkLightnessPercentage.value.value.isNaN ? 0.0 : changeFromNetworkLightnessPercentage.value.value
        changeFromNetworkLightnessPercentage.value = ReactiveResult(value: lightness, error: nil)
        let deltaUV = changeFromNetworkDeltaUv.value.value.isNaN ? -1.0 : changeFromNetworkDeltaUv.value.value
        changeFromNetworkDeltaUv.value = ReactiveResult(value: deltaUV, error: nil)
        
        let calculatedLightness = UInt16(lightness * Double(UInt16.max))
        let calculatedDeltaUV = Int16((deltaUV + 1.0) / 2.0 * Double(UInt16.max) - (Double(Int16.max) + 1))
        setCTL(lightness: calculatedLightness, temperature: temperature, deltaUV: calculatedDeltaUV)
    }
    
    private func changeDeltaUv(_ percentage: Double) {
        let deltaUV = Int16(percentage * Double(UInt16.max) - (Double(Int16.max) + 1))
        let lightness = changeFromNetworkLightnessPercentage.value.value.isNaN ? 0.0 : changeFromNetworkLightnessPercentage.value.value
        changeFromNetworkLightnessPercentage.value = ReactiveResult(value: lightness, error: nil)
        let temperature = changeFromNetworkTemperature.value.value.isNaN ? minTemperature : changeFromNetworkTemperature.value.value
        changeFromNetworkTemperature.value = ReactiveResult(value: temperature, error: nil)
        
        let calculatedLightness = UInt16(lightness * Double(UInt16.max))
        let calculatedTemperature = UInt16(temperature)
        setCTL(lightness: calculatedLightness, temperature: calculatedTemperature, deltaUV: deltaUV)
    }
    
    private func setCTL(lightness: UInt16, temperature: UInt16, deltaUV: Int16) {
        services.bluetoothMeshElementControl.set(colorTemperature: (Int(lightness), Int(temperature), Int(deltaUV)), for: device)
    }
}
