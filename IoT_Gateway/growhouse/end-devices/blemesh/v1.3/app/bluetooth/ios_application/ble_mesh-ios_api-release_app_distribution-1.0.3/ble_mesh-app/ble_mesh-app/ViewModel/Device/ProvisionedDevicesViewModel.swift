//
//  ProvisionedDevicesViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 20/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import RxSwift
import RxCocoa
import BluetoothMesh

class ProvisionedDevicesViewModel: BaseViewModel {
    var isConnected = Variable(false)
    var values = [Int : NSNumber]()
    let meshNetwork: SBMMeshNetwork
    let meshGroup: SBMMeshGroup?
    let devices = Variable<[ProvisionedDevicesCellViewModel]>([])
    
    init(_ services: ViewModelServices, meshNetwork: SBMMeshNetwork, meshGroup: SBMMeshGroup?) {
        self.meshNetwork = meshNetwork
        self.meshGroup = meshGroup

        super.init(services)
        
        NotificationCenter.default.addObserver(self, selector: #selector(nodesUpdated(notification:)), name: MeshNotifications.NodesUpdated, object: nil)
        updateDevices()
    }
    
    func updateDevices() {
        devices.value = services.networkManager.getNodes(withNetwork: meshNetwork)
        .filter {
            filter(node: $0)
        } .map {
            ProvisionedDevicesCellViewModel(meshNode: $0)
        }
    }
    
    func remove(device: SBMMeshNode, completion: @escaping (Bool) -> ()) {
        services.bluetoothMeshService.bleMeshManager.factoryReset(device) { (success) in
            NotificationCenter.default.post(name: MeshNotifications.NodesUpdated, object: nil)
            completion(success)
        }
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
    
    private func filter(node: SBMMeshNode) -> Bool {
        guard let meshGroup = meshGroup, let configuration = node.configuration else {
            return true
        }

        return configuration.applicationKeys.contains {$0.key == meshGroup.applicationKeyIndex }
    }
    
    @objc func nodesUpdated(notification: Notification){
        updateDevices()
    }
}
