//
//  FunctionalityViewModelProvider.swift
//  ble_mesh-app
//
//  Created by Łukasz Rzepka on 30/01/2019.
//  Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import Foundation
import BluetoothMesh

class FunctionalityViewModelProvider: BaseViewModel {
    
    func createViewModels(for devices: [ProvisionedDevicesCellViewModel]) -> [BaseViewModel?] {
        let functionalityViewModels = devices.enumerated().map { index, device -> BaseViewModel in
            let activeFunctionality = services.bluetoothMeshNodeFunctionality.activeFunctionality(for: device.node)
            let functionality = (activeFunctionality != nil) ? SBMModelIdentifier(rawValue: activeFunctionality!) : nil
            return viewModel(for: functionality, device: device, index: index)
        }
        
        return functionalityViewModels
    }
    
    private func viewModel(for functionality: SBMModelIdentifier?, device: ProvisionedDevicesCellViewModel, index: Int) -> BaseViewModel {
        guard let functionality = functionality else {
            return UnknownDeviceViewModel(services, name: device.name!, index: "\(index)")
        }
        
        switch functionality {
        case .genericOnOffServer,
             .genericPowerOnOffServer:
            return SwitchDeviceViewModel(services, device: device.node, index: "\(index)", isOn: false)
        case .genericLevelServer:
            return LevelDeviceViewModel(services, device: device.node, index: "\(index)", percentage: 0.0)
        case .lightLightnessServer:
            return LightDeviceViewModel(services, device: device.node, index: "\(index)", percentage: 0.0)
        case .lightCTLServer:
            return CtlDeviceViewModel(services, device: device.node, index: "\(index)", lightnessPercentage: 0.0, temperaturePercentage: 0.0, deltaUvPercentage: 0.0)
        case .genericOnOffClient,
             .genericPowerOnOffClient,
             .lightLightnessClient,
             .lightCTLClient,
             .genericLevelClient:
            return ClientDeviceViewModel(services, device: device.node, index: "\(index)")
        default:
            return UnknownDeviceViewModel(services, name: device.name!, index: "\(index)")
        }
    }
}
