//
//  BluetoothMeshNodeFunctionality.swift
//  ble_mesh-app
//
//  Created by Piotr Sarna on 18/12/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import BluetoothMesh

class BluetoothMeshNodeFunctionality {
    
    typealias NodeFunctionalityCallback = (_ errors: [Error?]) -> ()
    
    var supportedModels: [SBMModelIdentifier] {
        return [.genericOnOffServer,
                .genericOnOffClient,
                .genericLevelServer,
                .genericLevelClient,
                .lightLightnessServer,
                .lightLightnessClient,
                .lightCTLServer,
                .lightCTLClient,
                .lightCTLTemperatureServer]
    }
    
    private var additionalModels: [SBMModelIdentifier: [SBMModelIdentifier]] {
        return [.genericOnOffServer : [.lightLightnessServer],
                .genericOnOffClient : [.lightLightnessClient],
                .lightLightnessServer : [.genericOnOffServer],
                .lightLightnessClient : [.genericOnOffClient],
                .genericLevelServer : [.genericOnOffServer],
                .genericLevelClient : [.genericOnOffClient],
                .lightCTLServer : [.lightCTLTemperatureServer, .genericOnOffServer, .lightLightnessServer],
                .lightCTLTemperatureServer : [.lightCTLServer, .genericOnOffServer, .lightLightnessServer],
                .lightCTLClient : [.genericOnOffClient, .lightLightnessClient]]
    }
    
    private var functionalities = [String : Int]()
    private var plistPath: String {
        let documentDirectory = NSSearchPathForDirectoriesInDomains(.documentDirectory, .userDomainMask, true).first! as String
        return documentDirectory.appending("/functionalities.plist")
    }
    private let lockQueue = DispatchQueue(label: "lockQueue")
    
    init() {
        functionalities = read()
    }
    
    func manageModelBindingsChange(from fromModel: SBMSigModel, to toModel: SBMSigModel, node: SBMNode, group: SBMGroup, completion: NodeFunctionalityCallback? = nil) {
        guard let from = additionalModels[fromModel.modelIdentifier], let to = additionalModels[toModel.modelIdentifier] else {
            let error = NSError(domain: "com.silabs.applications.bluetooth.mesh.BluetoothMeshNodeFunctionality", code: 0, userInfo: nil)
            completion?([error])
            return
        }
        let fromModels = from + [fromModel.modelIdentifier]
        let toModels = to + [toModel.modelIdentifier]
        
        let modelsToUnbind = fromModels
            .filter { !toModels.contains($0) }
            .compactMap { modelToUnbind in
                node.elements.flatMap { $0.sigModels }.first { $0.modelIdentifier == modelToUnbind } }
        let modelsToBind = toModels
            .filter { !fromModels.contains($0) }
            .compactMap { modelToBind in
                node.elements.flatMap { $0.sigModels }.first { $0.modelIdentifier == modelToBind } }
        
        let dispatchGroup = DispatchGroup()
        var errors = [Error?]()
        modelsToUnbind.forEach { model in
            dispatchGroup.enter()
            unbind(model: model, from: group) {
                errors += $0
                dispatchGroup.leave()
            }
        }
        modelsToBind.forEach { model in
            dispatchGroup.enter()
            bind(model: model, to: group) {
                errors += $0
                dispatchGroup.leave()
            }
        }
        
        dispatchGroup.notify(queue: .main) {
            self.set(activeFunctionality: toModel.identifier, for: node)
            completion?(errors)
        }
    }
    
    func bindWithAdditionalModels(model: SBMSigModel, in node: SBMNode, to group: SBMGroup, completion: NodeFunctionalityCallback? = nil) {
        var errors = [Error?]()
        let dispatchGroup = DispatchGroup()
        
        dispatchGroup.enter()
        bind(model: model, to: group) {
            errors += $0
            dispatchGroup.leave()
        }
        
        additionalModels[model.modelIdentifier]?
            .compactMap { additionalModel in node.elements.flatMap { $0.sigModels }.first { $0.modelIdentifier == additionalModel } }
            .forEach {
                dispatchGroup.enter()
                bind(model: $0, to: group) {
                    errors += $0
                    dispatchGroup.leave()
                }
        }
        
        dispatchGroup.notify(queue: .main) {
            self.set(activeFunctionality: model.identifier, for: node)
            completion?(errors)
        }
    }
    
    func bind(model: SBMSigModel, to group: SBMGroup, completion: NodeFunctionalityCallback? = nil) {
        Logger.log(message: "Bind model: \(model.modelIdentifier) to group: \(group.name)")
        let functionalityBinder = SBMFunctionalityBinder(group: group)
        let subscriptionControl = SBMSubscriptionControl(model: model)
        let subscriptionSettings = SBMSubscriptionSettings(group: group)
        let publicationSettings = SBMPublicationSettings(group: group)
        publicationSettings.ttl = 5
        let dispatchGroup = DispatchGroup()
        
        var errors = [Error?]()
        
        dispatchGroup.enter()
        functionalityBinder.bindModel(model, successCallback: { _, _ in dispatchGroup.leave() }, errorCallback: { _, _, error in
            errors.append(error)
            dispatchGroup.leave()
        })
        
        if model.supportSubscribe {
            dispatchGroup.enter()
            Logger.log(message: "Add group: \(group.name) subscription settings to model: \(model.modelIdentifier)")
            subscriptionControl.add(subscriptionSettings, successCallback: { _, _ in dispatchGroup.leave() }, errorCallback: { _, _, error in
                errors.append(error)
                dispatchGroup.leave()
            })
        }
        
        if model.supportPublish {
            dispatchGroup.enter()
            Logger.log(message: "Set group: \(group.name) publication settings to model: \(model.modelIdentifier)")
            subscriptionControl.setPublicationSettings(publicationSettings, successCallback: { _, _ in dispatchGroup.leave() }, errorCallback: { _, _, error in
                errors.append(error)
                dispatchGroup.leave()
            })
        }
        
        dispatchGroup.notify(queue: .main) {
            Logger.log(message: "Bind model: \(model.modelIdentifier) to group: \(group.name) finished with errors: \(errors.compactMap { $0?.localizedDescription })")
            completion?(errors)
        }
    }
    
    func unbindWithAdditionalModels(model: SBMSigModel, in node: SBMNode, from group: SBMGroup, completion: NodeFunctionalityCallback? = nil) {
        var errors = [Error?]()
        let dispatchGroup = DispatchGroup()
        
        dispatchGroup.enter()
        unbind(model: model, from: group) {
            errors += $0
            dispatchGroup.leave()
        }
        
        additionalModels[model.modelIdentifier]?
            .compactMap { additionalModel in node.elements.flatMap { $0.sigModels }.first { $0.modelIdentifier == additionalModel } }
            .forEach {
                dispatchGroup.enter()
                unbind(model: $0, from: group) {
                    errors += $0
                    dispatchGroup.leave()
                }
        }
        
        dispatchGroup.notify(queue: .main) {
            self.removeActiveFunctionality(for: node)
            completion?(errors)
        }
    }
    
    func unbind(model: SBMSigModel, from group: SBMGroup, completion: NodeFunctionalityCallback? = nil) {
        Logger.log(message: "Unbind model: \(model.modelIdentifier) from group: \(group.name)")
        let functionalityBinder = SBMFunctionalityBinder(group: group)
        let subscriptionControl = SBMSubscriptionControl(model: model)
        let dispatchGroup = DispatchGroup()
        
        var errors = [Error?]()
        
        dispatchGroup.enter()
        Logger.log(message: "Clear publication settings from model: \(model.modelIdentifier)")
        subscriptionControl.clearPublicationSettings({ _, _ in dispatchGroup.leave() }, errorCallback: { _, _, error in
            errors.append(error)
            dispatchGroup.leave()
        })
        
        dispatchGroup.enter()
        Logger.log(message: "Clear subscription settings from model: \(model.modelIdentifier)")
        subscriptionControl.remove(SBMSubscriptionSettings(group: group), successCallback: { _, _ in dispatchGroup.leave() }, errorCallback: { _, _, error in
            errors.append(error)
            dispatchGroup.leave()
        })
        
        dispatchGroup.enter()
        functionalityBinder.unbindModel(model, successCallback: { _, _ in dispatchGroup.leave() }, errorCallback: { _, _, error in
            errors.append(error)
            dispatchGroup.leave()
        })
        
        dispatchGroup.notify(queue: .main) {
            Logger.log(message: "Unbind model: \(model.modelIdentifier) from group: \(group.name) finished with errors: \(errors.compactMap { $0?.localizedDescription })")
            completion?(errors)
        }
    }
    
    func activeFunctionality(for node: SBMNode) -> Int? {
        return lockQueue.sync { functionalities[node.uuid.hexEncodedString()] }
    }
    
    func removeActiveFunctionality(for node: SBMNode) {
        lockQueue.sync {
            Logger.log(message: "Remove active functionality for node: \(node.name)")
            functionalities.removeValue(forKey: node.uuid.hexEncodedString())
            save()
            NotificationCenter.default.post(name: BluetoothMeshNotifications.NodesUpdated, object: nil)
        }
    }
    
    private func set(activeFunctionality functionality: Int, for node: SBMNode) {
        lockQueue.sync {
            Logger.log(message: "Set active functionality for node: \(node.name)")
            functionalities[node.uuid.hexEncodedString()] = functionality
            save()
            NotificationCenter.default.post(name: BluetoothMeshNotifications.NodesUpdated, object: nil)
        }
    }
    
    private func save() {
        NSDictionary(dictionary: functionalities).write(toFile: plistPath, atomically: true)
    }
    
    private func read() -> [String : Int] {
        return NSDictionary(contentsOfFile: plistPath) as? [String : Int] ?? [:]
    }
}
