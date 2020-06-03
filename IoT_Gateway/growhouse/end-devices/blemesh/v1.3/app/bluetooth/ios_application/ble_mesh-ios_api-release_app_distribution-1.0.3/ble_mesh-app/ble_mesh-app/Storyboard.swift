//
//  Storyboard.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 25.04.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import Foundation

struct Storyboard {
    struct DeviceConfiguration {
        struct Segue {
            static let configurationTabbar = "configurationTabbar"
            static let selectGroup = "selectGroup"
            static let selectFunctionality = "selectFunctionality"
            static let editName = "editName"
            static let elements = "elements"
        }
        
        struct CellIdentifiers {
            static let infoCell = "infoCell"
            static let selectionCell = "selectionCell"
            static let elementCell = "elementCell"
        }
    }
    
    struct Main {
        struct Segue {
            
        }
    }
    
    struct Network {
        struct Segue {
            static let addNetwork = "addNetwork"
            static let network = "network"
            static let tabbar = "tabbar"
            static let deviceConfiguration = "deviceConfiguration"
            static let addGroup = "addGroup"
            static let showGroup = "showGroup"
            static let showDevices = "showDevices"
            static let showMasterControl = "showMasterControl"
            static let networkConfiguration = "networkConfiguration"
            static let editNetworkName = "editNetworkName"
            static let groupConfiguration = "groupConfiguration"
            static let editGroupName = "editGroupName"
        }
        
        struct CellIdentifiers {
            static let networkCell = "networkCell"
            static let groupCell = "groupCell"
            static let lightDeviceCell = "lightDeviceCell"
            static let levelDeviceCell = "levelDeviceCell"
            static let switchDeviceCell = "switchDeviceCell"
            static let unknownDeviceCell = "unknownDeviceCell"
            static let clientDeviceCell = "clientDeviceCell"
        }
    }
    
    struct Provision {
        struct Segue {
            static let deviceConfiguration = "deviceConfiguration"
            static let selectNetwork = "selectNetwork"
            static let deviceProvisioning = "deviceProvisioning"
        }
    }
}
