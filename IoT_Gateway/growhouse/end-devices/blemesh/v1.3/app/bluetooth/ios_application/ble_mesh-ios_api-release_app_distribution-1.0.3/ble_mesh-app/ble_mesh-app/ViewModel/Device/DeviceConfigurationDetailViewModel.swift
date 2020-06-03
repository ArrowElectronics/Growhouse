//
//  DeviceConfigurationDetailViewModel.swift
//  ble_mesh-app
//
//  Created by Lukasz Rzepka on 20.04.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import Foundation
import BluetoothMesh
import SVProgressHUD
import RxSwift
import RxCocoa

class DeviceConfigurationDetailViewModel: BaseViewModel {
    var device: SBMMeshNode
    var groupName = Variable(String())
    var modelFunctionalityName = Variable(String())
    var onDeinit: () -> ()
    var isProxy = Variable<Bool>(false)
    var isRelay = Variable<Bool>(false)
    var isFriend = Variable<Bool>(false)
    var supportFriend = Variable<Bool>(false)
    var supportLowPower = Variable<Bool>(false)
    
    private var group: SBMMeshGroup?
    private var model: SBMModel?
    
    init(_ services: ViewModelServices, device: SBMMeshNode) {
        self.device = device
        self.onDeinit = { }

        super.init(services)

        refreshDeviceInfo()
    }
    
    deinit {
        onDeinit()
    }
    
    func refreshDeviceInfo() {
        self.device = self.services.networkManager.meshNode(withUUID: self.device.meshDeviceUUID)!
        self.groupName.value = self.services.networkManager.group(withMeshNode: self.device)?.name ?? ""
        self.modelFunctionalityName.value = getFunctionalityName() ?? ""
        
        self.services.bluetoothMeshService.deviceCompositionData(for: self.device, completion: {[weak self] (dcd, error) in
            guard error == nil, dcd != nil else {
                return
            }
            
            self?.supportFriend.value = dcd!.friendSupport()
            self?.supportLowPower.value = dcd!.lowPowerSupport()
        })
        
        NotificationCenter.default.post(name: MeshNotifications.NodesUpdated, object: nil)
    }
    
    private func getFunctionalityName() -> String? {
        guard let config = self.device.configuration,
            let activeFunctionality = config.activeFunctionality,
            let models = config.configuredModels(forElementIndex: activeFunctionality.functionalityElementId) else {
            return nil
        }
        
        let activeModel = models.first { $0.identifier.rawValue == activeFunctionality.functionalityId.rawValue }
        
        return activeModel?.name
    }
    
    func checkNodeConfiguration() {
        SVProgressHUD.show(withStatus: "Loading Proxy state")
        
        services.bluetoothMeshService.bleMeshManager.checkProxy(for: device) { proxy, success in
            if success {
                self.isProxy.value = proxy
                SVProgressHUD.showSuccess(withStatus: "Proxy updated")
            } else {
                SVProgressHUD.showError(withStatus: "Proxy failed to update")
            }
            
            DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 1, execute: {
                self.checkForRelay()
            })
        }
    }
    
    func checkForRelay() {
        SVProgressHUD.show(withStatus: "Loading Relay state")
        self.services.bluetoothMeshService.bleMeshManager.checkRelay(for: self.device) { relay, success in
            if success {
                self.isRelay.value = relay
                SVProgressHUD.showSuccess(withStatus: "Relay updated")
            } else {
                SVProgressHUD.showError(withStatus: "Relay failed to update")
            }
            SVProgressHUD.dismiss(withDelay: 1)
            
            DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 1, execute: {
                self.checkForFriend()
            })
        }
    }
    
    func checkForFriend() {
        self.services.bluetoothMeshService.deviceCompositionData(for: self.device, completion: { dcd, error in
            guard error == nil, dcd != nil else {
                return
            }
            
            guard dcd!.friendSupport() == true else {
                return
            }
            
            SVProgressHUD.show(withStatus: "Loading Friend state")
            self.services.bluetoothMeshService.bleMeshManager.checkFriend(for: self.device, completion: { friend, success in
                if success {
                    self.isFriend.value = friend
                    SVProgressHUD.showSuccess(withStatus: "Friend updated")
                } else {
                    SVProgressHUD.showError(withStatus: "Friend failed to update")
                }
                SVProgressHUD.dismiss(withDelay: 1)
            })
        })
    }
    
    func changeProxyTo(_ isProxy: Bool) {
        SVProgressHUD.show(withStatus: isProxy ? "Turning Proxy On..." : "Turning Proxy Off...")
        services.bluetoothMeshService.bleMeshManager.setProxy(device, enable: isProxy) { success in
            if success {
                SVProgressHUD.showSuccess(withStatus: "Proxy updated")
            } else {
                SVProgressHUD.showError(withStatus: "Proxy failed to update")
            }
            
            SVProgressHUD.dismiss(withDelay: 2)
        }
    }
    
    func changeRelayTo(_ isRelay: Bool) {
        SVProgressHUD.show(withStatus: isRelay ? "Turning Relay On..." : "Turning Relay Off...")
        services.bluetoothMeshService.bleMeshManager.setRelay(device, enable: isRelay) { success in
            if success {
                SVProgressHUD.showSuccess(withStatus: "Relay updated")
            } else {
                SVProgressHUD.showError(withStatus: "Relay failed to update")
            }
            
            SVProgressHUD.dismiss(withDelay: 2)
        }
    }
    
    func changeFriendTo(_ isFriend: Bool) {
        SVProgressHUD.show(withStatus: isFriend ? "Turning Friend On..." : "Turning Friend Off...")
        services.bluetoothMeshService.bleMeshManager.setFriend(device, enable: isFriend) { success in
            if success {
                SVProgressHUD.showSuccess(withStatus: "Friend updated")
            } else {
                SVProgressHUD.showError(withStatus: "Friend failed to update")
            }
            
            SVProgressHUD.dismiss(withDelay: 2)
        }
    }
    
    func changeNameTo(_ name: String) {
        services.bluetoothMeshService.bleMeshManager.change(device, name: name)
        NotificationCenter.default.post(name: MeshNotifications.NodesUpdated, object: nil)
    }
    
    func groupPickerViewModel() -> OptionPickerViewModel {
        var selectedGroup : OptionPickerItemViewModel?
        let network = services.networkManager.network(withMeshNode: device)!
        let options = services.networkManager.getGroups(withNetwork: network).map { (meshGroup) -> OptionPickerItemViewModel in
            let item = OptionPickerItemViewModel(identifier: String(meshGroup.applicationKeyIndex), name: meshGroup.name, data: meshGroup)
            
            guard let configuration = device.configuration else {
                return item
            }
            
            let nodeContainAppKey : Bool? = configuration.applicationKeys.contains { $0.key == Int(meshGroup.applicationKeyIndex) }
            
            if nodeContainAppKey == true {
                selectedGroup = item
            }
            
            return item
        }
        
        let oldGroup = selectedGroup?.data as? SBMMeshGroup
        var newGroup = oldGroup
        
        return OptionPickerViewModel(options: options, selectedOption: selectedGroup, changeSelectionCallback: { (selectedGroup) in
            newGroup = selectedGroup?.data as? SBMMeshGroup
        }, onDeinitialization: { [weak self] in
            guard let `self` = self else { return }
            
            let selectGroupForFirstTime = oldGroup == nil && newGroup != nil
            let changeToDifferentGroup = oldGroup?.applicationKeyIndex != newGroup?.applicationKeyIndex
            
            if selectGroupForFirstTime || changeToDifferentGroup {
                SVProgressHUD.show(withStatus: "Changing group...")
                
                if selectGroupForFirstTime {
                    self.services.bluetoothMeshService.bleMeshManager.add(self.device, to: newGroup!) { (success) in
                        if (success) {
                            SVProgressHUD.showSuccess(withStatus: "Group has been changed")
                        } else {
                            SVProgressHUD.showError(withStatus: "Error changing group")
                        }
                        
                        SVProgressHUD.dismiss(withDelay: 2)
                        self.refreshDeviceInfo()
                    }
                } else {
                    self.services.bluetoothMeshService.bleMeshManager.remove(self.device, from: oldGroup!) { (success) in
                        if (success) {
                            self.services.bluetoothMeshService.bleMeshManager.add(self.device, to: newGroup!) { (success) in
                                if (success) {
                                    SVProgressHUD.showSuccess(withStatus: "Group has been changed")
                                } else {
                                    SVProgressHUD.showError(withStatus: "Error changing group")
                                }
                                
                                SVProgressHUD.dismiss(withDelay: 2)
                                self.refreshDeviceInfo()
                            }
                        } else {
                            SVProgressHUD.showError(withStatus: "Error changing group")
                            SVProgressHUD.dismiss(withDelay: 2)
                        }
                        
                        self.refreshDeviceInfo()
                    }
                }
            }
        })
    }
    
    func functionalityPickerViewModel() -> OptionPickerViewModel {
        var selectedFunctionality : OptionPickerItemViewModel?
        guard let node = services.networkManager.meshNode(withUUID: device.meshDeviceUUID) else {
            return OptionPickerViewModel(options: [], selectedOption: nil, changeSelectionCallback: {_ in }, onDeinitialization: {})
        }
        
        print("CONFIGURED MODELS: ", node.configuration?.configuredModels(forElementIndex: 0) ?? "")
        
        let options = node.configuration?.configuredModels(forElementIndex: 0)?.map {(model) -> OptionPickerItemViewModel in
            let item = OptionPickerItemViewModel(identifier: String(model.identifier.rawValue), name: model.name, data: model)
            
            if model.identifier == node.configuration?.activeFunctionality?.functionalityId {
                selectedFunctionality = item
            }
            
            return item
        }
        
        return OptionPickerViewModel(options: options ?? [], selectedOption: selectedFunctionality, changeSelectionCallback: { [weak self] (newFunctionality) in
            self?.model = newFunctionality?.data as? SBMModel
            self?.modelFunctionalityName.value = (self?.model?.name)!
            }, onDeinitialization: { [weak self] in
                let configuredFunctionalities = self?.services.bluetoothMeshService.bleMeshManager.configuredFunctionalities(for: node)
                guard let functionality = configuredFunctionalities?.first(where: { (deviceFunctionality) -> Bool in
                    return deviceFunctionality.functionalityId.rawValue == self?.model?.identifier.rawValue
                }) else {
                    return
                }
                
                self?.services.bluetoothMeshService.bleMeshManager.change(node, functionality: functionality)
                NotificationCenter.default.post(name: MeshNotifications.NodesUpdated, object: nil)
        })
    }
}
