//
//  OptionPickerItemViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 23/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import Foundation

class OptionPickerItemViewModel {
    
    let identifier: String
    let name: String
    let data: Any?
    
    init(identifier: String, name: String, data: Any?) {
        self.identifier = identifier
        self.name = name
        self.data = data
    }
}
