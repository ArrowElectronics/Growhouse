//
//  NetworksViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 12/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import RxSwift
import RxCocoa
import BluetoothMesh
import SVProgressHUD

class NetworksViewModel: BaseViewModel {
    
    let subnets = Variable([NetworkCellViewModel]())
    let canCreateSubnet = Variable(false)
    private var connectionProvider: NetworkConnectionProvider?
    
    override init(_ services: ViewModelServices) {
        super.init(services)
        
        NotificationCenter.default.addObserver(self, selector: #selector(updateNetworks(notification:)), name: BluetoothMeshNotifications.NetworksUpdated, object: nil)
        NotificationCenter.default.addObserver(self, selector: #selector(updateNetworks(notification:)), name: BluetoothMeshNotifications.NodesUpdated, object: nil)
        NotificationCenter.default.addObserver(self, selector: #selector(updateNetworks(notification:)), name: BluetoothMeshNotifications.GroupsUpdated, object: nil)
        updateNetworks()
    }
    
    @objc func updateNetworks(notification: Notification?) {
        updateNetworks()
    }
    
    func updateNetworks() {
        subnets.value = services.bluetoothMeshNetworkManager.subnets.map { NetworkCellViewModel(subnet: $0, deviceCount: $0.nodes.count, groupCount: $0.groups.count) }
        canCreateSubnet.value = services.bluetoothMeshNetworkManager.canCreateSubnet()
    }
    
    func remove(subnet: SBMSubnet, completion: ((_ success: Bool, _ status: String) -> (Void))? = nil) {
        for node in subnet.nodes {
            services.bluetoothMeshNodeFunctionality.removeActiveFunctionality(for: node)
        }
        
        var onceToken = true
        let connectionCallback: NetworkConnectionCallback = { [weak self] _, _, error in
            if onceToken {
                onceToken = false
                self?.handleConnectionCallback(for: subnet, error: error, completion: completion)
            }
        }
        connectionProvider = NetworkConnectionProvider(withSubnet: subnet,
                                                       bluetoothMeshService: services.bluetoothMeshService,
                                                       bluetoothManager: services.bluetoothManager,
                                                       connectionCallback: connectionCallback)
        connectionProvider?.establishConnection()
    }
    
    private func handleConnectionCallback(for subnet: SBMSubnet, error: NetworkConnectionProviderError?, completion: ((_ success: Bool, _ status: String) -> (Void))?) {
        guard error == nil else {
            if case let NetworkConnectionProviderError.unknownError(networkError) = error! {
                completion?(false, networkError.localizedDescription)
            } else {
                completion?(false, error!.errorDescription)
            }
            return
        }
        services.bluetoothMeshNetworkManager.remove(subnet: subnet) { (nodes) -> (Void) in
            if let nodes = nodes {
                completion?(false, "Following nodes could not be reset: \(nodes.map { $0.name })")
            } else {
                completion?(true, "Success")
            }
        }
    }
    
    func forceRemove(subnet subnetViewModel: NetworkCellViewModel) {
        for node in subnetViewModel.subnet.nodes {
            services.bluetoothMeshNodeFunctionality.removeActiveFunctionality(for: node)
        }
        
        services.bluetoothMeshNetworkManager.forceRemoveSubnet(subnetViewModel.subnet)
    }
    
    func exportURL() -> URL? {
        return services.bluetoothMeshDatabaseExport.export()
    }
    
    func clearExport(url: URL) -> Bool {
        return services.bluetoothMeshDatabaseExport.removeExport(url: url)
    }
    
    func reset() {
        services.bluetoothMeshNetworkManager.dropDatabase()
    }
}
