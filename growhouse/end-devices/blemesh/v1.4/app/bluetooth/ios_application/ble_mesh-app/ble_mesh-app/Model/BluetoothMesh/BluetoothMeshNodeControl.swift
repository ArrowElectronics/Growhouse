//
//  BluetoothMeshNodeControl.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 16/01/2019.
//  Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import BluetoothMesh

class BluetoothMeshNodeControl {
    
    typealias BindGroupCallback = (_ error: Error?) -> ()
    
    private let nodeFunctionality: BluetoothMeshNodeFunctionality
    
    init(nodeFunctionality: BluetoothMeshNodeFunctionality) {
        self.nodeFunctionality = nodeFunctionality
    }
    
    func bind(node: SBMNode, to group: SBMGroup, completion: BindGroupCallback? = nil) {
        Logger.log(message: "Bind node: \(node.name) to group: \(group.name)")
        let nodeControl = SBMNodeControl(node: node)
        nodeControl.bind(to: group, successCallback: {
            Logger.log(message: "Success binding node: \(node.name) to group: \(group.name)")
            completion?(nil)
        }, errorCallback: {
            Logger.log(message: "Error: \($0.localizedDescription), while binding node: \(node.name) to group: \(group.name)")
            completion?($0)
        })
    }
    
    func unbind(node: SBMNode, from group: SBMGroup) {
        Logger.log(message: "Unbind node: \(node.name) from group: \(group.name)")
        if let activeFunctionality = nodeFunctionality.activeFunctionality(for: node), let model = node.elements.flatMap({ $0.sigModels }).first(where: { $0.identifier == activeFunctionality }) {
            nodeFunctionality.unbindWithAdditionalModels(model: model, in: node, from: group)
        }
        let nodeControl = SBMNodeControl(node: node)
        nodeControl.unbind(from: group, successCallback: {
            Logger.log(message: "Success unbinding node: \(node.name) from group: \(group.name)")
        }, errorCallback: {
            Logger.log(message: "Error: \($0.localizedDescription), while unbinding node: \(node.name) from group: \(group.name)")
        })
    }
}
