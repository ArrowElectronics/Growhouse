//
//  NetworkDetailViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 19/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import SVProgressHUD
import BluetoothMesh
import RxSwift
import CoreBluetooth

class NetworkDetailViewModel: BaseViewModel {
    let subnet: SBMSubnet
    let provisionedDevicesViewModel : ProvisionedDevicesViewModel
    let groupsViewModel: GroupsViewModel
    var networkConnectionProvider: NetworkConnectionProvider?
    var networkMatchResults = [BluetoothDevice]()
    var connectedPeripheral: BluetoothDevice?

    init(_ services: ViewModelServices, subnet: SBMSubnet) {
        self.subnet = subnet
        provisionedDevicesViewModel = ProvisionedDevicesViewModel(services, subnet: subnet, group: nil)
        groupsViewModel = GroupsViewModel(services, subnet: subnet)
        
        super.init(services)
    }
    
    deinit {
        disconnect()
    }
    
    func disconnect() {
        if let connectedPeripheral = connectedPeripheral {
            services.bluetoothMeshService.disconnect(device: connectedPeripheral)
        }
    }
    
    func connect(completion: ((Bool, String) -> ())? = nil) {
        let connectionCallback: NetworkConnectionCallback = { [weak self] (peripheralConnection, _, error) in
            self?.handleConnectionCallback(with: peripheralConnection, error: error, completion: completion)
        }
        networkConnectionProvider = NetworkConnectionProvider(withSubnet: subnet,
                                                              bluetoothMeshService: services.bluetoothMeshService,
                                                              bluetoothManager: services.bluetoothManager,
                                                              connectionCallback: connectionCallback)
        networkConnectionProvider?.establishConnection()
    }
    
    private func handleConnectionCallback(with device: BluetoothDevice?, error: NetworkConnectionProviderError?, completion: ((Bool, String) -> ())?) {
        guard error == nil else {
            if case let NetworkConnectionProviderError.unknownError(networkError) = error! {
                completion?(false, networkError.localizedDescription)
            } else {
                completion?(false, error!.errorDescription)
            }
            return
        }
        
        self.connectedPeripheral = device
        NotificationCenter.default.post(name: BluetoothMeshNotifications.NodesUpdated, object: nil)
        completion?(true, "Connected")
    }
}
