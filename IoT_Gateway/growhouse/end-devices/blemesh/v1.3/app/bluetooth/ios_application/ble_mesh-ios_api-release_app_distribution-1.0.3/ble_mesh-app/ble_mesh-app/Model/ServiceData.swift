//
//  ServiceData.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 22.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import CoreBluetooth

class ServiceData {
    
    private(set) var data: Data
    private(set) var serviceDataLength: UInt8
    private(set) var serviceDataType: UInt8
    private(set) var meshProxyService: UInt16
    
    init?(withDictionary dictionary: NSDictionary) {
        guard let serviceData = dictionary[CBUUID(string: MeshConstants.meshAdvertimentDataKeyForProxyDataService)] as? NSData else {
            return nil
        }

        serviceDataLength = ServiceData.dataLength(for: serviceData)
        serviceDataType = ServiceData.dataType
        meshProxyService = ServiceData.meshProxy
        
        data = Data()
        data.append(Data(bytes: &serviceDataLength, count: MemoryLayout.size(ofValue: serviceDataLength)))
        data.append(Data(bytes: &serviceDataType, count: MemoryLayout.size(ofValue: serviceDataType)))
        data.append(Data(bytes: &meshProxyService, count: MemoryLayout.size(ofValue: meshProxyService)))
        data.append(Data(referencing: serviceData))
    }
    
    private static func dataLength(for serviceData: NSData) -> UInt8 {
        return UInt8(serviceData.length + 3)
    }
    
    private static var dataType: UInt8 {
        return 0x16
    }
    
    private static var meshProxy: UInt16 {
        return 0x1828
    }
}
