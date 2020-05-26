//
//  BluetoothMeshGroupControl.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 18/12/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import BluetoothMesh

class BluetoothMeshGroupControl {
    
    typealias SetCallback = (Error?) -> (Void)

    func set(onOff: Bool, for group: SBMGroup, completion: SetCallback? = nil) {
        let controlGroup = SBMControlGroup(group: group)
        let status = SBMGenericOnOff(onOffState: onOff ? .on : .off)
        let parameters = SBMControlRequestParameters(transitionTime: 1, delayTime: 1, requestReplay: false)
        Logger.log(message: "Set onOff to: \(onOff) for group: \(group.name)")
        controlGroup.setStatus(status, parameters: parameters, successCallback: { _, response in
            let on = (response as? SBMGenericOnOff)?.onOffState == .on
            Logger.log(message: "Success setting onOff to: \(on) for group: \(group.name)")
            completion?(nil)
        }, errorCallback: { _, _, error in
            Logger.log(message: "Error: \(error.localizedDescription), while setting onOff for group: \(group.name)")
            completion?(error)
        })
    }
    
    func set(lightness: Int, for group: SBMGroup, completion: SetCallback? = nil) {
        let controlGroup = SBMControlGroup(group: group)
        let status = SBMLightningLightnessActual(lightness: UInt16(lightness))
        let parameters = SBMControlRequestParameters(transitionTime: 1, delayTime: 1, requestReplay: false)
        Logger.log(message: "Set lightness to: \(lightness) for group: \(group.name)")
        controlGroup.setStatus(status, parameters: parameters, successCallback: { _, response in
            let lightness = (response as? SBMLightningLightnessActual)?.lightness ?? 0
            Logger.log(message: "Success setting lightness to: \(lightness) for group: \(group.name)")
            completion?(nil)
        }, errorCallback: { _, _, error in
            Logger.log(message: "Error: \(error.localizedDescription), while setting lightness for group: \(group.name)")
            completion?(error)
        })
    }
}
