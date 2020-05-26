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
        return button(color: UIColor(red: 255.0/255.0, green: 25.0/255.0, blue: 42.0/255.0, alpha: 1.0), image: #imageLiteral(resourceName: "ic_delete"))
    }
    
    static var configureButton: UIButton {
        return button(color: UIColor(red: 142.0/255.0, green: 142.0/255.0, blue: 147.0/255.0, alpha: 1.0), image: #imageLiteral(resourceName: "ic_config"))
    }
    
    private static func button(color: UIColor, image: UIImage) -> UIButton {
        let button = UIButton(type: .custom)
        
        button.backgroundColor = color
        button.setImage(image, for: .normal)
        
        return button
    }
}
