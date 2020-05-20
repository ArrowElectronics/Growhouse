//
//  UIAlertController+Custom.swift
//  ble_mesh-app
//
//  Created by Natalia Niziołek on 10/09/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import UIKit

extension UIAlertController {
    
    convenience init(title: String?, message: String?, cancelTitle: String = "Cancel", destructiveTitle: String = "Ok", completion: ((Bool) -> ())? = nil) {
        let cancel = UIAlertAction(title: cancelTitle, style: .cancel) { _ in completion?(false) }
        let destructive = UIAlertAction(title: destructiveTitle, style: .destructive) { _ in completion?(true) }
        
        self.init(title: title, message: message, preferredStyle: .alert)
        
        addAction(cancel)
        addAction(destructive)
    }
}
