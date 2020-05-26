//
//  ProvisionedDevicesViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 20/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import RxSwift
import RxCocoa
import BluetoothMesh

class ProvisionedDevicesViewModel: BaseViewModel {
    
    var values = [Int : NSNumber]()
    let subnet: SBMSubnet
    let group: SBMGroup?
    let devices = Variable([ProvisionedDevicesCellViewModel]())
    let functionalityViewModelProvider: FunctionalityViewModelProvider
    
    init(_ services: ViewModelServices, subnet: SBMSubnet, group: SBMGroup?) {
        self.subnet = subnet
        self.group = group
        self.functionalityViewModelProvider = FunctionalityViewModelProvider.init(services)

        super.init(services)
        
        NotificationCenter.default.addObserver(self, selector: #selector(nodesUpdated), name: BluetoothMeshNotifications.NodesUpdated, object: nil)
        NotificationCenter.default.addObserver(self, selector: #selector(groupValueChanged), name: BluetoothMeshNotifications.GroupValueChanged, object: nil)
        updateDevices()
    }
    
    func remove(device: SBMNode, completion: @escaping (SBMNode, Error?) -> ()) {
        services.bluetoothMeshNodeFunctionality.removeActiveFunctionality(for: device)
        services.bluetoothMeshConfigurationControl.factoryReset(for: device) { completion($0, $1) }
    }
    
    func forceRemove(device: SBMNode) {
        services.bluetoothMeshNodeFunctionality.removeActiveFunctionality(for: device)
        services.bluetoothMeshNetworkManager.forceRemoveNode(device)
    }
    
    func updateValues(for row: Int) {
        values.removeValue(forKey: row)
        var newValues = [Int : NSNumber]()
        for value in values {
            if (value.key < row) {
                newValues[value.key] = value.value
            } else {
                newValues[value.key - 1] = value.value
            }
        }
        values = newValues
    }
    
    func getFunctionalityViewModels() -> [BaseViewModel?] {
         return functionalityViewModelProvider.createViewModels(for: devices.value)
    }
    
    @objc private func nodesUpdated(notification: Notification){
        updateDevices()
    }
    
    @objc private func groupValueChanged(notification: Notification) {
        let value = notification.userInfo?["value"] as! Double
        for indice in devices.value.indices {
            values[indice] = NSNumber(value: value)
        }
    }
    
    private func updateDevices() {
        devices.value = subnet.nodes
            .filter {
                guard let group = group else { return true }
                return $0.groups.contains { $0.appKey == group.appKey } }
            .map { ProvisionedDevicesCellViewModel(node: $0) }
    }
}
