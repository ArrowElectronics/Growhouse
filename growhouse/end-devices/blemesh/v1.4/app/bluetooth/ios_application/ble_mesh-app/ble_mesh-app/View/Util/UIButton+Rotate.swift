//
//  UIButton+Rotate.swift
//  ble_mesh-app
//
//  Created by Łukasz Rzepka on 01/02/2019.
//  Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import Foundation
import UIKit

extension UIButton {
    
    func startRotation(duration: CFTimeInterval = 1.0) {
        let rotation = CABasicAnimation(keyPath: "transform.rotation")
        rotation.fromValue = 0
        rotation.toValue = 2.0 * .pi
        rotation.duration = duration
        rotation.repeatCount = .greatestFiniteMagnitude
        
        layer.add(rotation, forKey: "button.rotate")
    }
    
    func stopRotation() {
        layer.removeAnimation(forKey: "button.rotate")
    }
    
}
