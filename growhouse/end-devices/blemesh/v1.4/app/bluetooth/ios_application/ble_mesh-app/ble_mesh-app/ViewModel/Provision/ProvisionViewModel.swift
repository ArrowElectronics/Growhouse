//
//  ProvisionViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 12/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import RxSwift
import RxCocoa
import BluetoothMesh

class ProvisionViewModel: BaseViewModel, DeviceProvisioningViewModelDelegate {
    
    let devices : Observable<[ProvisionCellViewModel]>
    let didProvisionNodeSubject = PublishSubject<(SBMNode, BluetoothDevice)>()
    private(set) var bluetoothConnected = Variable(false)
    var canProvisionDevice: Bool {
        return services.bluetoothMeshNetworkManager.subnets.first != nil
    }
    private let disposeBag = DisposeBag()
    
    override init(_ services: ViewModelServices) {
        devices = services.bluetoothManager.rx
            .observe([UUID : BluetoothDevice].self, "discoveredDevices")
            .map { $0?.values.map { ProvisionCellViewModel(services: services, device: $0) } ?? [] }

        super.init(services)
        
        services.bluetoothManager.bluetoothConnected.asObservable().bind(to: bluetoothConnected).disposed(by: disposeBag)
    }
    
    func deviceProvisioningViewModel(for device: BluetoothDevice) -> DeviceProvisioningViewModel? {
        let viewModel = DeviceProvisioningViewModel(services, device: device, subnet: services.bluetoothMeshNetworkManager.subnets.first!)
        viewModel.delegate = self
        return viewModel
    }
    
    func scanForDevices() {
        services.bluetoothManager.startDiscovering(services: [SBMProvisionerConnection.meshProvisioningServiceUUID()])
    }
    
    func stopScanForDevices() {
        services.bluetoothManager.stopDiscovering()
    }
    
    func didProvisionDevice(node: SBMNode, device: BluetoothDevice?) {
        guard let device = device else { return }
        services.bluetoothMeshNodeFunctionality.removeActiveFunctionality(for: node)
        didProvisionNodeSubject.onNext((node, device))
        NotificationCenter.default.post(name: BluetoothMeshNotifications.NetworksUpdated, object: nil)
    }
}
