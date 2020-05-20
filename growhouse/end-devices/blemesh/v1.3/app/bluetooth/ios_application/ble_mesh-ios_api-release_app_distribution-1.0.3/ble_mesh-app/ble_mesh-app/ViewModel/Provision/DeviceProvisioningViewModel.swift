//
//  DeviceProvisioningViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 13/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import Foundation
import RxSwift
import RxCocoa
import BluetoothMesh
import SVProgressHUD

protocol DeviceProvisioningViewModelDelegate: class {
    func didProvisionDevice(node: SBMMeshNode, connection: PeripheralConnection?)
}

class DeviceProvisioningViewModel : BaseViewModel {
    let name : Variable<String?>
    let canProvisionDevice : Observable<Bool>
    let device : PeripheralConnection
    let networkName : Variable<String>
    var delegate: DeviceProvisioningViewModelDelegate?
    private var network : SBMMeshNetwork
    
    init(_ services: ViewModelServices, device: PeripheralConnection, network:SBMMeshNetwork) {
        self.device = device
        self.network = network
        networkName = Variable<String>(network.name)
        
        name = Variable<String?>(device.name)
        canProvisionDevice = name.asObservable().map { !($0 ?? "").trimmingCharacters(in: .whitespacesAndNewlines).isEmpty }
        
        super.init(services)
    }
    
    func done() {
        SVProgressHUD.show(withStatus: "Provisioning...")

        services.bluetoothMeshService.provisionPeripheralConnection(device, withName: name.value!, toNetworkIndex: network.index) { [weak self] (maybeNode, maybeError) in
            if maybeError != nil || maybeNode == nil  {
                self?.services.bluetoothMeshService.closeMeshNodeConnectionWith(self?.device)
                SVProgressHUD.showError(withStatus: maybeError?.errorDescription)
                SVProgressHUD.dismiss(withDelay: 2)
                return
            }
            
            SVProgressHUD.showSuccess(withStatus: "Provision success")
            SVProgressHUD.dismiss(withDelay: 1) {
                self?.delegate?.didProvisionDevice(node: maybeNode!, connection: self?.device)
            }
        }
    }
    
    func networkPickerViewModel() -> OptionPickerViewModel {
        var selectedNetwork : OptionPickerItemViewModel?
        
        let options = services.networkManager.getNetworks().map { (meshNetwork) -> OptionPickerItemViewModel in
            let item = OptionPickerItemViewModel(identifier: String(meshNetwork.index), name: meshNetwork.name, data: meshNetwork)
            
            if meshNetwork.index == network.index {
                selectedNetwork = item
            }
            
            return item
        }
        
        return OptionPickerViewModel(options: options, selectedOption: selectedNetwork, changeSelectionCallback: { [weak self] (newNetwork) in
            self?.network = newNetwork?.data as! SBMMeshNetwork
            self?.networkName.value = (self?.network.name)!
        }, onDeinitialization: { })
    }
}
