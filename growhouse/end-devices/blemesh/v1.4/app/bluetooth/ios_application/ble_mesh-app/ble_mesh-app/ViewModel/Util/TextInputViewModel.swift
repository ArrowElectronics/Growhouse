//
//  TextInputViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 11/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

 class TextInputViewModel {
    
    let didReturn: (String?) -> ()
    var placeholder: String?
    var text: String?
    
    init(withReturnCallback callback: @escaping (String?) -> ()) {
        didReturn = callback
        placeholder = nil
        text = nil
    }
}
