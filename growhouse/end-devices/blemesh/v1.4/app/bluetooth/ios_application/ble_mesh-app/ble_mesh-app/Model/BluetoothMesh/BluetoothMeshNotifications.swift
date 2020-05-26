//
//  BluetoothMeshNotifications.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 25/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import Foundation

class BluetoothMeshNotifications {
    
    static let NetworksUpdated = Notification.Name("MeshNetworksUpdated")
    static let NodesUpdated = Notification.Name("MeshNodesUpdated")
    static let GroupsUpdated = Notification.Name("MeshGroupsUpdated")
    static let GroupValueChanged = Notification.Name("GroupValueChanged")
}
