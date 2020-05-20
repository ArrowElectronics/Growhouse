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
    let networks = Variable<[NetworkCellViewModel]>([])
    
    override init(_ services: ViewModelServices) {
        super.init(services)
        
        NotificationCenter.default.addObserver(self, selector: #selector(updateNetworks(notification:)), name: MeshNotifications.NetworksUpdated, object: nil)
        NotificationCenter.default.addObserver(self, selector: #selector(updateNetworks(notification:)), name: MeshNotifications.NodesUpdated, object: nil)
        NotificationCenter.default.addObserver(self, selector: #selector(updateNetworks(notification:)), name: MeshNotifications.GroupsUpdated, object: nil)
        updateNetworks()
    }
    
    @objc func updateNetworks(notification: Notification?) {
        updateNetworks()
    }
    
    func updateNetworks() {
        networks.value = services.networkManager.getNetworks().map { (network) in
            let deviceCount = services.networkManager.getNodes(withNetwork: network).count
            let groupCount = services.networkManager.getGroups(withNetwork: network).count;
            
            return NetworkCellViewModel(network: network, deviceCount: deviceCount, groupCount: groupCount)
        }
    }
    
    func remove(network: SBMMeshNetwork) {
        guard services.networkManager.getNodes(withNetwork: network).count > 0 else {
            services.networkManager.removeNetwork(network)
            SVProgressHUD.showSuccess(withStatus: "Network removed")
            SVProgressHUD.dismiss(withDelay: 1.5)
            return
        }
        
        SVProgressHUD.show(withStatus: "Removing...")
        services.peripheralDiscovery.startDiscovery(forServiceUUIDs: [meshProxyServiceUUID])
        DispatchQueue.main.asyncAfter(deadline: .now() + 2.0) { [weak self] in
            if let weakSelf = self {
                let networkMatchResults = weakSelf.getMatchResults(in: network)
                weakSelf.services.peripheralDiscovery.stopDiscovery()
                weakSelf.remove(network: network, with: networkMatchResults)
            }
        }
    }
    
    private func remove(network: SBMMeshNetwork, with connections: [PeripheralConnection]) {
        connectToNetwork(network, with: connections) { [weak self] gattHandle in
            guard let weakSelf = self, gattHandle != nil else {
                SVProgressHUD.showError(withStatus: "Failed to remove network")
                SVProgressHUD.dismiss(withDelay: 1.5)
                return
            }
            
            let nodes = weakSelf.services.networkManager.getNodes(withNetwork: network)
            var numberOfNodes = nodes.count
            for node in nodes {
                weakSelf.services.bluetoothMeshService.bleMeshManager.factoryReset(node) { success in
                    numberOfNodes -= 1
                    if numberOfNodes == 0 {
                        weakSelf.services.networkManager.removeNetwork(network)
                        SVProgressHUD.showSuccess(withStatus: "Network removed")
                        SVProgressHUD.dismiss(withDelay: 1.5)
                    }
                }
            }
        }
    }
    
    private func getMatchResults(in network: SBMMeshNetwork) -> [PeripheralConnection] {
        var networkMatchResults = [PeripheralConnection]()
        
        for peripheral in services.peripheralDiscovery.discoveredPeripherals.values {
            if let matchResult = filter(peripheral: peripheral, in: network) {
                networkMatchResults.append(matchResult)
            }
        }
        
        return networkMatchResults
    }
    
    private func filter(peripheral: PeripheralConnection, in network: SBMMeshNetwork) -> PeripheralConnection? {
        let serviceDataDictionary = peripheral.advertisementData["kCBAdvDataServiceData"] as! NSDictionary
        guard let serviceData = ServiceData(withDictionary: serviceDataDictionary) else {
            return nil
        }
        
        if services.bluetoothMeshService.bleMeshManager.networkHashMatches(Int(network.index), advertisingData: serviceData.data) {
            return peripheral
        }
        
        return nil
    }
    
    private func connectToNetwork(_ network: SBMMeshNetwork, with connections: [PeripheralConnection], completion: @escaping (Int?) -> ()) {
        var completion: ((Int?) -> ())? = completion
        let sortedConnections = connections.sorted { $0.rssi?.compare($1.rssi!) == .orderedDescending }
        
        guard let connection = sortedConnections.first else {
            completion?(nil)
            return
        }
        
        services.bluetoothMeshService.startProxyConnectionWith(connection) { [weak self] gattHandle, error in
            if error != nil {
                self?.services.bluetoothMeshService.closeMeshNodeConnectionWith(connection)
            }
            
            completion?(gattHandle)
            completion = nil
        }
    }
}
