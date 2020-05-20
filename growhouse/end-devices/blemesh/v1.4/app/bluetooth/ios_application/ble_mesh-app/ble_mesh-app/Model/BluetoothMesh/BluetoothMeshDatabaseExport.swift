//
//  BluetoothMeshDatabaseExport.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 28/01/2019.
//  Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import Foundation
import BluetoothMesh

class BluetoothMeshDatabaseExport {
    
    private let bluetoothMesh: SBMBluetoothMesh
    
    private var path: URL? {
        guard let path = NSSearchPathForDirectoriesInDomains(.cachesDirectory, .userDomainMask, true).first else {
            return nil
        }
        
        return URL(fileURLWithPath: path).appendingPathComponent("MeshDictionary.json")
    }
    
    init(bluetoothMesh: SBMBluetoothMesh) {
        self.bluetoothMesh = bluetoothMesh
    }
    
    func export() -> URL? {
        Logger.log(message: "Export started")
        let json = SBMExportKeysJob(bluetoothMesh: bluetoothMesh).fire()
        guard let meshJson = json.jsonString else {
            Logger.log(message: "Could not serialize json: \(json)")
            return nil
        }
        
        guard let url = path else {
            Logger.log(message: "Could not get path for export directory")
            return nil
        }
        
        do {
            try meshJson.write(to: url, atomically: false, encoding: .utf8)
        } catch {
            Logger.log(message: "Could not create file: \(url.absoluteString)")
            return nil
        }
        
        Logger.log(message: "Export finished")
        return url
    }
    
    func removeExport(url: URL) -> Bool {
        do {
            try FileManager.default.removeItem(at: url)
        } catch {
            Logger.log(message: "Could not remove file: \(url.absoluteString)")
            return false
        }
        
        return true
    }
}
