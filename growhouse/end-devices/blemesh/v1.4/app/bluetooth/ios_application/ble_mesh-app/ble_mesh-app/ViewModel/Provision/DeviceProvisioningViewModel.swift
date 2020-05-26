//
//  DeviceProvisioningViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 13/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import RxSwift
import RxCocoa
import BluetoothMesh
import SVProgressHUD

protocol DeviceProvisioningViewModelDelegate: class {
    func didProvisionDevice(node: SBMNode, device: BluetoothDevice?)
}

class DeviceProvisioningViewModel : BaseViewModel {
    
    typealias DeviceProvisioningCallback = (_ node: SBMNode?, _ device: BluetoothDevice?, _ error: Error?) -> ()
    
    let name: String
    let device: BluetoothDevice
    let networkName: Variable<String>
    var delegate: DeviceProvisioningViewModelDelegate?
    private var subnet: SBMSubnet
    
    init(_ services: ViewModelServices, device: BluetoothDevice, subnet: SBMSubnet) {
        self.device = device
        self.subnet = subnet
        networkName = Variable(subnet.name)
        name = device.name()
        
        super.init(services)
    }
    
    var networkPickerViewModel: OptionPickerViewModel {
        let options = services.bluetoothMeshNetworkManager.subnets.map {
            OptionPickerItemViewModel(identifier: String($0.netKey.rawKey.hexEncodedString()), name: $0.name, data: $0)
        }
        let selectedOption = options.first { $0.identifier == String(subnet.netKey.rawKey.hexEncodedString()) }
        let changeSelectionCallback: OptionPickerViewModel.SelectionCallback = { [weak self] newSubnet in
            self?.subnet = newSubnet.data as! SBMSubnet
            self?.networkName.value = (self?.subnet.name)!
        }
        
        return OptionPickerViewModel(options: options, selectedOption: selectedOption, changeSelectionCallback: changeSelectionCallback)
    }
    
    func canProvision() -> Bool {
        return services.bluetoothMeshService.node(for: device) == nil
    }
    
    func provision(completion: DeviceProvisioningCallback? = nil) {
        services.bluetoothMeshService.provision(device: device, to: subnet) { [weak self] node, error in
            if let error = error {
                completion?(nil, nil, error)
            } else {
                completion?(node, self?.device, nil)
            }
        }
    }
    
    func removeDevice() {
        if let node = services.bluetoothMeshService.node(for: device) {
            services.bluetoothMeshNetworkManager.forceRemoveNode(node)
        }
    }
}
