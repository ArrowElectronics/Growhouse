//
//  DeviceConfigurationDetailViewModel.swift
//  ble_mesh-app
//
//  Created by Lukasz Rzepka on 20.04.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import BluetoothMesh
import SVProgressHUD
import RxSwift
import RxCocoa

class DeviceConfigurationDetailViewModel: BaseViewModel {
    
    var device: SBMNode
    var groupName = Variable(String())
    var modelFunctionalityName = Variable(String())
    var onDeinit: () -> ()
    var isProxy = Variable(false)
    var isRelay = Variable(false)
    var isFriend = Variable(false)
    var supportFriend = Variable(false)
    var supportLowPower = Variable(false)
    
    private var model: SBMModel?
    
    init(_ services: ViewModelServices, device: SBMNode) {
        self.device = device
        self.onDeinit = { }

        super.init(services)
        
        getDCD()
        refreshDeviceInfo()
    }
    
    deinit {
        onDeinit()
    }
    
    private func getDCD() {
        services.bluetoothMeshConfigurationControl.getDeviceCompositionData(for: device) { [weak self] dcd, _, _ in
            if let dcd = dcd {
                self?.supportFriend.value = dcd.supportsFriend()
                self?.supportLowPower.value = dcd.isLowPower()
            }
        }
    }
    
    private func refreshDeviceInfo() {
        device = services.bluetoothMeshNetworkManager.node(withUUID: device.uuid)!
        groupName.value = device.groups.first?.name ?? ""
        modelFunctionalityName.value = device.elements.compactMap { $0.sigModels.first { $0.identifier == functionality } }.first?.name ?? ""
    }
    
    func checkConfiguration(completion: @escaping (Bool) -> ()) {
        checkProxy() { [weak self] successProxy in
            self?.checkRelay() { [weak self] successRelay in
                self?.checkFriend { successFriend in
                    completion(successProxy && successRelay && successFriend)
                }
            }
        }
    }
    
    private func checkProxy(completion: @escaping (Bool) -> ()) {
        services.bluetoothMeshConfigurationControl.getProxy(for: device) { [weak self] enabled, error in
            self?.set(self?.isProxy, enabled: enabled, error: error, completion: completion)
        }
    }
    
    private func checkRelay(completion: @escaping (Bool) -> ()) {
        services.bluetoothMeshConfigurationControl.getRelay(for: device) { [weak self] enabled, retransmissionCount, retransmissionInterval, error in
            self?.set(self?.isRelay, enabled: enabled, error: error, completion: completion)
        }
    }
    
    private func checkFriend(completion: @escaping (Bool) -> ()) {
        services.bluetoothMeshConfigurationControl.getFriend(for: device) { [weak self] (enabled, error) in
            self?.set(self?.isFriend, enabled: enabled, error: error, completion: completion)
        }
    }
    
    private func set(_ variable: Variable<Bool>?, enabled: Bool?, error: Error?, completion: @escaping (Bool) -> ()) {
        if let enabled = enabled {
            variable?.value = enabled
        }
        completion(error == nil)
    }
    
    func setProxy(_ enabled: Bool, completion: @escaping (Bool) -> ()) {
        services.bluetoothMeshConfigurationControl.setProxy(enabled, for: device) { error in
            if error != nil && !enabled {
                self.services.bluetoothMeshService.disconnect(device: self.services.bluetoothMeshService.activeDevice.value)
            }
            completion(error == nil)
        }
    }
    
    func setRelay(_ enabled: Bool, completion: @escaping (Bool) -> ()) {
        services.bluetoothMeshConfigurationControl.setRelay(enabled, retransmissionCount: 0, retransmissionInterval: 0, for: device) { error in
            completion(error == nil)
        }
    }
    
    func setFriend(_ enabled: Bool, completion: @escaping (Bool) -> ()) {
        services.bluetoothMeshConfigurationControl.setFriend(enabled, for: device) { error in
            completion(error == nil)
        }
    }
    
    func changeNameTo(_ name: String) {
        device.name = name
        NotificationCenter.default.post(name: BluetoothMeshNotifications.NodesUpdated, object: nil)
    }
    
    func groupPickerViewModel() -> OptionPickerViewModel {
        var selectedGroup : OptionPickerItemViewModel?
        
        let options = device.subnets.first!.groups.map { (group) -> OptionPickerItemViewModel in
            let item = OptionPickerItemViewModel(identifier: String(group.appKey.keyIndex), name: group.name, data: group)
            
            if device.groups.contains(where: { $0.appKey == group.appKey }) {
                selectedGroup = item
            }
            
            return item
        }
        
        let oldGroup = selectedGroup?.data as? SBMGroup
        var newGroup = oldGroup
        
        return OptionPickerViewModel(options: options, selectedOption: selectedGroup, changeSelectionCallback: { (selectedGroup) in
            newGroup = selectedGroup.data as? SBMGroup
        }, onDeinitialization: { [weak self] in
            self?.processGroupChanging(oldGroup: oldGroup, newGroup: newGroup)
        })
    }
    
    private func processGroupChanging(oldGroup: SBMGroup?, newGroup: SBMGroup?) {
        guard let newGroup = newGroup, oldGroup?.appKey != newGroup.appKey else { return }
        
        SVProgressHUD.show(withStatus: "Changing group...")
        
        if let oldGroup = oldGroup {
            services.bluetoothMeshNodeControl.unbind(node: device, from: oldGroup)
        }
        
        services.bluetoothMeshNodeControl.bind(node: device, to: newGroup) { [weak self] error in
            if let error = error {
                SVProgressHUD.showError(withStatus: error.localizedDescription)
            } else {
                SVProgressHUD.showSuccess(withStatus: "Group changed")
            }
            SVProgressHUD.dismiss(withDelay: 1.5)
            NotificationCenter.default.post(name: BluetoothMeshNotifications.NodesUpdated, object: nil)
            self?.refreshDeviceInfo()
        }
    }
    
    func functionalityPickerViewModel() -> OptionPickerViewModel {
        guard let sigModels = device.elements.first?.sigModels else {
            return OptionPickerViewModel(options: [], selectedOption: nil)
        }
        
        let filteredSigModels = sigModels.filter { services.bluetoothMeshNodeFunctionality.supportedModels.contains($0.modelIdentifier) }
        
        print("CONFIGURED MODELS: ", filteredSigModels.map { String($0.identifier) })
        
        let currentlySelectedFunctionality = functionality
        let options = filteredSigModels.map { OptionPickerItemViewModel(identifier: String($0.identifier), name: $0.name, data: $0) }
        let selectedFunctionality = options.first { ($0.data as! SBMModel).identifier == currentlySelectedFunctionality }
        let onSelectionChange: (OptionPickerItemViewModel?) -> Void = { [weak self] (newFunctionality) in
            guard let self = self else {
                return
            }
            
            self.model = newFunctionality?.data as? SBMModel
            self.modelFunctionalityName.value = self.model?.name ?? ""
        }
        let onDeinitialization: () -> Void = { [weak self] in
            guard let self = self, let model = self.model else {
                return
            }
            
            SVProgressHUD.show(withStatus: "Changing functionality...")
            if let oldFunctionality = selectedFunctionality?.data as? SBMSigModel {
                self.services.bluetoothMeshNodeFunctionality.unbindWithAdditionalModels(model: oldFunctionality, in: self.device, from: self.device.groups.first!) { _ in
                    self.services.bluetoothMeshNodeFunctionality.bindWithAdditionalModels(model: model as! SBMSigModel, in: self.device, to: self.device.groups.first!) { _ in
                        SVProgressHUD.showSuccess(withStatus: "Functionality changed")
                        SVProgressHUD.dismiss(withDelay: 1.5)
                    }
                }
            } else {
                self.services.bluetoothMeshNodeFunctionality.bindWithAdditionalModels(model: model as! SBMSigModel, in: self.device, to: self.device.groups.first!) { _ in
                    SVProgressHUD.showSuccess(withStatus: "Functionality changed")
                    SVProgressHUD.dismiss(withDelay: 1.5)
                }
            }
        }
        
        return OptionPickerViewModel(options: options, selectedOption: selectedFunctionality, changeSelectionCallback: onSelectionChange, onDeinitialization: onDeinitialization)
    }
    
    private var functionality: Int? {
        return services.bluetoothMeshNodeFunctionality.activeFunctionality(for: device)
    }
}
