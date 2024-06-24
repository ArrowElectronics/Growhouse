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
    let network: SBMMeshNetwork
    let provisionedDevicesViewModel : ProvisionedDevicesViewModel
    let groupsViewModel: GroupsViewModel
    var networkMatchResults = [PeripheralConnection]()
    var connectedPeripheral: PeripheralConnection?
    var isConnected = Variable(false)

    init(_ services: ViewModelServices, meshNetwork: SBMMeshNetwork) {
        network = meshNetwork
        provisionedDevicesViewModel = ProvisionedDevicesViewModel(services, meshNetwork: meshNetwork, meshGroup: nil)
        groupsViewModel = GroupsViewModel(services, meshNetwork: meshNetwork)
        
        super.init(services)
        
        if hasDevices {
            connect()
        }
    }
    
    deinit {
        disconnect()
    }
    
    func disconnect() {
        guard let connectedPeripheral = self.connectedPeripheral else {
            return
        }
        
        services.bluetoothMeshService.closeMeshNodeConnectionWith(connectedPeripheral)
        isConnected.value = false
    }
    
    private var hasDevices: Bool {
        return services.networkManager.getNodes(withNetwork: network).count > 0
    }
    
    func connect() {
        if hasDevices {
            SVProgressHUD.show(withStatus: "Connecting...")
            services.peripheralDiscovery.startDiscovery(forServiceUUIDs: [meshProxyServiceUUID])
            DispatchQueue.main.asyncAfter(deadline: .now() + 2.0) {
                for peripheral in self.services.peripheralDiscovery.discoveredPeripherals.values {
                    self.filter(peripheral: peripheral, in: self.network)
                }
                self.services.peripheralDiscovery.stopDiscovery()
                self.startPeripheralConnectionWithHighestRSSI()
            }
        } else {
            SVProgressHUD.showError(withStatus: "No devices in network")
            SVProgressHUD.dismiss(withDelay: 1.5)
        }
    }
    
    private func filter(peripheral: PeripheralConnection, in network: SBMMeshNetwork) {
        let serviceDataDictionary = peripheral.advertisementData["kCBAdvDataServiceData"] as! NSDictionary
        guard let serviceData = ServiceData(withDictionary: serviceDataDictionary) else {
            return
        }
        
        let networkMatch = networkCheck(for: peripheral, with: serviceData, in: network)
        
        if (networkMatch) {
            NotificationCenter.default.post(name: MeshNotifications.NodesUpdated, object: nil)
        }
    }
    
    private func networkCheck(for peripheral: PeripheralConnection, with serviceData: ServiceData, in network: SBMMeshNetwork) -> Bool {
        if services.bluetoothMeshService.bleMeshManager.networkHashMatches(Int(network.index), advertisingData: serviceData.data) {
            networkMatchResults.append(peripheral)
            return true
        }
        return false
    }
    
    private func startPeripheralConnectionWithHighestRSSI() {
        sortMatchResults()
        connectedPeripheral = getMatchResult()
        clearMatchResults()
        
        guard let peripheral = connectedPeripheral else {
            SVProgressHUD.showError(withStatus: "Peripheral not found")
            SVProgressHUD.dismiss(withDelay: 1.5)
            return
        }
        
        services.bluetoothMeshService.startProxyConnectionWith(peripheral) { [weak self] gattHandle, error  in
            if error != nil {
                self?.services.bluetoothMeshService.closeMeshNodeConnectionWith(peripheral)
            }
            self?.displayDialog(for: error)
        }
    }
    
    private func sortMatchResults() {
        networkMatchResults.sort { $0.rssi?.compare($1.rssi!) == .orderedDescending }
    }
    
    private func getMatchResult() -> PeripheralConnection? {
        var device: PeripheralConnection?
        
        if networkMatchResults.count > 0 {
            device = networkMatchResults.first!
        } else {
            device = nil
        }
        
        return device
    }
    
    private func clearMatchResults() {
        networkMatchResults.removeAll()
    }
    
    private func displayDialog(for error: LocalizedError?) {
        if error == nil {
            self.isConnected.value = true
            SVProgressHUD.showSuccess(withStatus: "Connected")
            SVProgressHUD.dismiss(withDelay: 1.5)
        } else {
            self.isConnected.value = false
            if !SVProgressHUD.isVisible() {
                SVProgressHUD.showError(withStatus: error?.errorDescription)
                SVProgressHUD.dismiss(withDelay: 1.5)
            }
        }
    }
}
