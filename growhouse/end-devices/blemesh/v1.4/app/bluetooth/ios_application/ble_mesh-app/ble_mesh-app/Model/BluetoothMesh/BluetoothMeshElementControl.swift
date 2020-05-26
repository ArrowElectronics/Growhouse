//
//  BluetoothMeshElementControl.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 18/12/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import BluetoothMesh

class BluetoothMeshElementControl {
    
    typealias SetCallback = (Error?) -> (Void)
    typealias GetOnOffCallback = (_ on: Bool?, _ error: Error?) -> ()
    typealias GetLevelCallback = (_ level: Int?, _ error: Error?) -> ()
    typealias GetLightnessCallback = (_ lightness: Int?, _ error: Error?) -> ()
    typealias GetColorTemepratureCallback = (_ lightness: Int?, _ temperature: Int?, _ error: Error?) -> ()
    typealias GetColorDeltaUvCallback = (_ temperature: Int?, _ deltaUv: Int?, _ error: Error?) -> ()
    
    func getOnOff(for node: SBMNode, completion: GetOnOffCallback? = nil) {
        Logger.log(message: "Get onOff for node: \(node.name)")
        guard let controlElement = controlElement(with: .genericOnOffServer, in: node) else {
            completion?(nil, NSError(domain: "Element for identifier \(SBMModelIdentifier.genericOnOffServer) not found", code: -1, userInfo: nil))
            return
        }
        
        controlElement.getStatus(SBMGenericOnOff.self, successCallback: { _, response in
            let response = response as! SBMGenericOnOff
            Logger.log(message: "Received onOff status: \(response.onOffState == .on)")
            completion?(response.onOffState == .on, nil)
        }, errorCallback: { _, _, error in
            Logger.log(message: "Get onOff status finished with error: \(error.localizedDescription)")
            completion?(nil, error)
        })
    }
    
    func set(onOff: Bool, for node: SBMNode, completion: SetCallback? = nil) {
        Logger.log(message: "Set onOff to: \(onOff) for node: \(node.name)")
        guard let controlElement = controlElement(with: .genericOnOffServer, in: node) else {
            completion?(NSError(domain: "Element for identifier \(SBMModelIdentifier.genericOnOffServer) not found", code: -1, userInfo: nil))
            return
        }
        
        let status = SBMGenericOnOff(onOffState: onOff ? .on : .off)
        let parameters = SBMControlRequestParameters(transitionTime: 1, delayTime: 1, requestReplay: false)
        controlElement.setStatus(status, parameters: parameters, successCallback: { _, response in
            let response = response as! SBMGenericOnOff
            Logger.log(message: "Did set onOff status to: \(response.onOffState == .on)")
            completion?(nil)
        }, errorCallback: { _, _, error in
            Logger.log(message: "Set onOff status finished with error: \(error.localizedDescription)")
            completion?(error)
        })
    }
    
    func getLevel(for node: SBMNode, completion: GetLevelCallback? = nil) {
        Logger.log(message: "Get level for node: \(node.name)")
        guard let controlElement = controlElement(with: .genericLevelServer, in: node) else {
            completion?(nil, NSError(domain: "Element for identifier \(SBMModelIdentifier.genericLevelServer) not found", code: -1, userInfo: nil))
            return
        }
        
        controlElement.getStatus(SBMGenericLevel.self, successCallback: { _, response in
            let response = response as! SBMGenericLevel
            Logger.log(message: "Received level status: \(response.level)")
            completion?(Int(response.level), nil)
        }, errorCallback: { _, _, error in
            Logger.log(message: "Get level status finished with error: \(error.localizedDescription)")
            completion?(nil, error)
        })
    }
    
    func set(level: Int, for node: SBMNode, completion: SetCallback? = nil) {
        Logger.log(message: "Set level to: \(level) for node: \(node.name)")
        guard let controlElement = controlElement(with: .genericLevelServer, in: node) else {
            completion?(NSError(domain: "Element for identifier \(SBMModelIdentifier.genericLevelServer) not found", code: -1, userInfo: nil))
            return
        }
        
        let status = SBMGenericLevel(level: Int16(level))
        let parameters = SBMControlRequestParameters(transitionTime: 1, delayTime: 1, requestReplay: false)
        controlElement.setStatus(status, parameters: parameters, successCallback: { _, response in
            let response = response as! SBMGenericLevel
            Logger.log(message: "Did set level status to: \(response.level)")
            completion?(nil)
        }, errorCallback: { _, _, error in
            Logger.log(message: "Set level status finished with error: \(error.localizedDescription)")
            completion?(error)
        })
    }
    
    func getLightness(for node: SBMNode, completion: GetLevelCallback? = nil) {
        Logger.log(message: "Get lightness for node: \(node.name)")
        guard let controlElement = controlElement(with: .lightLightnessServer, in: node) else {
            completion?(nil, NSError(domain: "Element for identifier \(SBMModelIdentifier.lightLightnessServer) not found", code: -1, userInfo: nil))
            return
        }
        
        controlElement.getStatus(SBMLightningLightnessActual.self, successCallback: { _, response in
            let response = response as! SBMLightningLightnessActual
            Logger.log(message: "Received lightness status: \(response.lightness)")
            completion?(Int(response.lightness), nil)
        }, errorCallback: { _, _, error in
            Logger.log(message: "Get lightness status finished with error: \(error.localizedDescription)")
            completion?(nil, error)
        })
    }
    
    func set(lightness: Int, for node: SBMNode, completion: SetCallback? = nil) {
        Logger.log(message: "Set lightness to: \(lightness) for node: \(node.name)")
        guard let controlElement = controlElement(with: .lightLightnessServer, in: node) else {
            completion?(NSError(domain: "Element for identifier \(SBMModelIdentifier.lightLightnessServer) not found", code: -1, userInfo: nil))
            return
        }
        
        let status = SBMLightningLightnessActual(lightness: UInt16(lightness))
        let parameters = SBMControlRequestParameters(transitionTime: 1, delayTime: 1, requestReplay: false)
        controlElement.setStatus(status, parameters: parameters, successCallback: { _, response in
            let response = response as! SBMLightningLightnessActual
            Logger.log(message: "Did set lightness status to: \(response.lightness)")
            completion?(nil)
        }, errorCallback: { _, _, error in
            Logger.log(message: "Set lightness status finished with error: \(error.localizedDescription)")
            completion?(error)
        })
    }
    
    func getColorTemperature(for node: SBMNode, completion: GetColorTemepratureCallback? = nil) {
        Logger.log(message: "Get color temperature for node: \(node.name)")
        guard let controlElement = controlElement(with: .lightCTLServer, in: node) else {
            completion?(nil, nil, NSError(domain: "Element for identifier \(SBMModelIdentifier.lightCTLServer) not found", code: -1, userInfo: nil))
            return
        }
        
        controlElement.getStatus(SBMLightningCTLGet.self, successCallback: { _, response in
            let response = response as! SBMLightningCTLGet
            Logger.log(message: "Received color temperature status: lightness: \(response.lightness), temperature: \(response.temperature)")
            completion?(Int(response.lightness), Int(response.temperature), nil)
        }, errorCallback: { _, _, error in
            Logger.log(message: "Get color temperature status finished with error: \(error.localizedDescription)")
            completion?(nil, nil, error)
        })
    }
    
    func set(colorTemperature: (lightness: Int, temperature: Int, deltaUv: Int), for node: SBMNode, completion: SetCallback? = nil) {
        Logger.log(message: "Set color temperature to: \(colorTemperature) for node: \(node.name)")
        guard let controlElement = controlElement(with: .lightCTLServer, in: node) else {
            completion?(NSError(domain: "Element for identifier \(SBMModelIdentifier.lightCTLServer) not found", code: -1, userInfo: nil))
            return
        }
        
        let status = SBMLightningCTLSet(lightness: UInt16(colorTemperature.lightness), temperature: UInt16(colorTemperature.temperature), deltaUv: Int16(colorTemperature.deltaUv))
        let parameters = SBMControlRequestParameters(transitionTime: 1, delayTime: 1, requestReplay: false)
        controlElement.setStatus(status, parameters: parameters, successCallback: { _, response in
            let response = response as! SBMLightningCTLSet
            Logger.log(message: "Did set color temperature status to: lightness: \(response.lightness), temperature: \(response.temperature), deltaUv: \(response.deltaUv)")
            completion?(nil)
        }, errorCallback: { _, _, error in
            Logger.log(message: "Set color temperature status finished with error: \(error.localizedDescription)")
            completion?(error)
        })
    }
    
    func getColorDeltaUv(for node: SBMNode, completion: GetColorTemepratureCallback? = nil) {
        Logger.log(message: "Get color delta uv for node: \(node.name)")
        guard let controlElement = controlElement(with: .lightCTLTemperatureServer, in: node) else {
            completion?(nil, nil, NSError(domain: "Element for identifier \(SBMModelIdentifier.lightCTLTemperatureServer) not found", code: -1, userInfo: nil))
            return
        }
        
        controlElement.getStatus(SBMLightningCTLTemperature.self, successCallback: { _, response in
            let response = response as! SBMLightningCTLTemperature
            Logger.log(message: "Received color delta uv status: temperature: \(response.temperature), deltaUV: \(response.deltaUv)")
            completion?(Int(response.temperature), Int(response.deltaUv), nil)
        }, errorCallback: { _, _, error in
            Logger.log(message: "Get color delta uv status finished with error: \(error.localizedDescription)")
            completion?(nil, nil, error)
        })
    }
    
    private func controlElement(with identifier: SBMModelIdentifier, in node: SBMNode) -> SBMControlElement? {
        let element = node.elements.first { (element) -> Bool in
            element.sigModels.contains { $0.modelIdentifier == identifier }
        }
        
        if let element = element, let group = node.groups.first {
            return SBMControlElement(element: element, in: group)
        } else {
            Logger.log(message: "Element for identifier: \(identifier) not found")
            return nil
        }
    }
}
