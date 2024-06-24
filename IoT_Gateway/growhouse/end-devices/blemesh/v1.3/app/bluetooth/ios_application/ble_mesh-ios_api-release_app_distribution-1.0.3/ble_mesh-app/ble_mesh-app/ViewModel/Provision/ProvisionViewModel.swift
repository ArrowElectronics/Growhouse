//
//  ProvisionViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 12/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import RxSwift
import RxCocoa
import BluetoothMesh

typealias ConnectProxyCallback = (Int, BluetoothMeshServiceError?) -> ()

class ProvisionViewModel: BaseViewModel, DeviceProvisioningViewModelDelegate {
    let devices : Observable<[ProvisionCellViewModel]>
    let didProvisionMeshNodeSubject = PublishSubject<(SBMMeshNode, PeripheralConnection)>()
    private(set) var bluetoothConnected = Variable(false)
    
    override init(_ services: ViewModelServices) {
        devices = services.peripheralDiscovery
            .rx.observe([UUID: PeripheralConnection].self, "discoveredPeripherals")
            .map({ (peripherals) -> [ProvisionCellViewModel] in
                return peripherals?.values.map({ (peripheral) -> ProvisionCellViewModel in
                    return ProvisionCellViewModel(services: services, peripheral: peripheral)
                }) ?? []
            })
        
        bluetoothConnected.value = services.peripheralDiscovery.bluetoothConnected

        super.init(services)
        
        NotificationCenter.default.addObserver(forName: MeshNotifications.BluetoothStateUpdated, object: nil, queue: .main) { [weak self] (notification) in
            if let btConnected = notification.userInfo?["isPoweredOn"] as? Bool {
                self?.bluetoothConnected.value = btConnected
            }
        }
    }
    
    func deviceProvisioningViewModel(forProvisionCellViewModel cell : ProvisionCellViewModel) -> DeviceProvisioningViewModel? {
        if let network = services.networkManager.getNetworks().first {
            let viewModel = DeviceProvisioningViewModel(services, device: cell.model, network: network)
            viewModel.delegate = self
            return viewModel
        } else {
            return nil
        }
    }
    
    func scanForDevices() {
        self.services.peripheralDiscovery.startDiscovery(forServiceUUIDs: [meshProvisioningServiceUUID])
    }
    
    func stopScanForDevices() {
        self.services.peripheralDiscovery.stopDiscovery()
    }
    
    func connectWithGattHandle(_ gattHandle: Int, callback: ConnectProxyCallback!) {
        guard let peripheralConnection = services.peripheralDiscovery.findPeripheralConnection(for: gattHandle) else {
            callback(gattHandle, BluetoothMeshServiceError.peripheralNotFound)
            return
        }
        
        services.bluetoothMeshService.startProxyConnectionWith(peripheralConnection) { [weak self] gattHandle, error in
            if error != nil {
                callback(gattHandle!, error)
                self?.services.bluetoothMeshService.closeMeshNodeConnectionWith(peripheralConnection)
                return
            }
            callback(gattHandle!, nil)
        }
    }
    
    func deviceProvisioningViewDidDismiss() {
        
    }
    
    func didProvisionDevice(node: SBMMeshNode, connection: PeripheralConnection?) {
        guard let peripheral = connection else {
            return
        }
        didProvisionMeshNodeSubject.onNext((node, peripheral))
    }
}
