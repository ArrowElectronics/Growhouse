//
//  Storyboard.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 25.04.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import Foundation

struct Storyboard {
    
    struct Device {
        struct Segue {
            static let deviceConfiguration = "deviceConfiguration"
        }
        
        struct Cell {
            static let ctlDeviceCell = "ctlDeviceCell"
            static let lightDeviceCell = "lightDeviceCell"
            static let levelDeviceCell = "levelDeviceCell"
            static let switchDeviceCell = "switchDeviceCell"
            static let unknownDeviceCell = "unknownDeviceCell"
            static let clientDeviceCell = "clientDeviceCell"
        }
    }
    
    struct DeviceConfiguration {
        struct Segue {
            static let configurationTabbar = "configurationTabbar"
            static let selectGroup = "selectGroup"
            static let selectFunctionality = "selectFunctionality"
            static let editName = "editName"
            static let elements = "elements"
        }
        
        struct Cell {
            static let elementCell = "elementCell"
        }
    }
    
    struct Group {
        struct Segue {
            static let addGroup = "addGroup"
            static let showGroup = "showGroup"
            static let showDevices = "showDevices"
            static let showMasterControl = "showMasterControl"
            static let groupConfiguration = "groupConfiguration"
            static let editGroupName = "editGroupName"
        }
        
        struct Cell {
            static let groupCell = "groupCell"
        }
    }
    
    struct Network {
        struct Segue {
            static let addNetwork = "addNetwork"
            static let network = "network"
            static let tabbar = "tabbar"
            static let networkConfiguration = "networkConfiguration"
            static let editNetworkName = "editNetworkName"
        }
        
        struct Cell {
            static let networkCell = "networkCell"
        }
    }
    
    struct Provision {
        struct Segue {
            static let deviceConfiguration = "deviceConfiguration"
            static let selectNetwork = "selectNetwork"
            static let deviceProvisioning = "deviceProvisioning"
        }
        
        struct Cell {
            static let provisionCell = "provisionCell"
        }
    }
    
    struct Common {
        struct Cell {
            static let optionCell = "optionCell"
        }
    }
}
