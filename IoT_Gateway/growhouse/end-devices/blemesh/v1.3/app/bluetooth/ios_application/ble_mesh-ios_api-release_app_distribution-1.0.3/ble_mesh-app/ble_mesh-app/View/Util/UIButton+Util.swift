//
//  UIButton+Util.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 05.06.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit

extension UIButton {
    
    static var deleteButton: UIButton {
        let deleteButton = UIButton.init(type: .custom)
        
        deleteButton.backgroundColor = UIColor(red: 255.0/255.0, green: 25.0/255.0, blue: 42.0/255.0, alpha: 1.0)
        deleteButton.setImage(#imageLiteral(resourceName: "ic_delete"), for: .normal)
        
        return deleteButton
    }
    
    static var configureButton: UIButton {
        let configureButton = UIButton.init(type: .custom)
        
        configureButton.backgroundColor = UIColor.init(red: 142.0/255.0, green: 142.0/255.0, blue: 147.0/255.0, alpha: 1.0)
        configureButton.setImage(#imageLiteral(resourceName: "ic_config"), for: .normal)
        
        return configureButton
    }
}
