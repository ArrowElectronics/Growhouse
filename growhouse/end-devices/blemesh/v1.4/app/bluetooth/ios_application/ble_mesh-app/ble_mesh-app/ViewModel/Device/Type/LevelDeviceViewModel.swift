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

class LevelDeviceViewModel: BaseViewModel, UpdatableViewModel {
    
    let device: SBMNode
    let index: String
    
    let changeFromNetworkPercentage: Variable<ReactiveResult<Float>>
    private let toChangeByViewPercentage: Variable<Float>
    
    private let disposeBag = DisposeBag()
    
    init(_ services: ViewModelServices, device: SBMNode, index: String, percentage: Float) {
        self.device = device
        self.index = index
        self.changeFromNetworkPercentage = Variable(ReactiveResult(value: Float.nan, error: nil))
        self.toChangeByViewPercentage = Variable(percentage)
        super.init(services)
        
        self.addObservers()
    }
    
    func setLevel(value: Float) {
        changeFromNetworkPercentage.value = ReactiveResult(value: value, error: nil)
        toChangeByViewPercentage.value = value
    }
    
    func getValuesFromNetwork(completion: (() -> ())? = nil) {
        services.bluetoothMeshElementControl.getLevel(for: device) { [weak self] (level, error) in
            guard error == nil, let level = level else {
                self?.handleErrorFromNetwork(error: error)
                return
            }
            
            let calculatedLevel = Float(level + Int(Int16.max) + 1) / Float(UInt16.max)
            self?.changeFromNetworkPercentage.value = ReactiveResult(value: calculatedLevel, error: nil)
        }
    }
    
    private func handleErrorFromNetwork(error: Error?) {
        let errorToSend: Error
        
        if error == nil {
            errorToSend = NSError(domain: "com.silabs.bluetoothmesh.level.notKnown", code: -1, userInfo: nil)
        } else {
            errorToSend = error!
        }
        
        changeFromNetworkPercentage.value = ReactiveResult(value: Float.nan, error: errorToSend)
    }
    
    private func addObservers() {
        self.toChangeByViewPercentage.asObservable()
            .buffer(timeSpan: 0.5, count: 0, scheduler: MainScheduler.instance)
            .map { $0.last }
            .filter { $0 != nil }
            .skip(1)
            .subscribe(onNext: { [weak self] (value) in
                self?.change(percentage: value!)
            })
            .disposed(by: disposeBag)
    }
    
    private func change(percentage: Float) {
        var status = Int(percentage * Float(UInt16.max)) - Int(Int16.max)
        if status > Int16.max {
            status = Int(Int16.max)
        }
        
        services.bluetoothMeshElementControl.set(level: status, for: device)
    }
}
