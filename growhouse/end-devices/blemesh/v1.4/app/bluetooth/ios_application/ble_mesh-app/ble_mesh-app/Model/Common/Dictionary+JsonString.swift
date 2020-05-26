//
//  Dictionary+JsonString.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 12/12/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import Foundation

extension Dictionary {
    
    var jsonString: String? {
        if let jsonData = try? JSONSerialization.data(withJSONObject: self, options: []) {
            return String(data: jsonData, encoding: .utf8)
        }
        
        return nil
    }
}
