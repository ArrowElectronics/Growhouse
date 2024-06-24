//
//  PixelConstraint.swift
//  ble_mesh-app
//
//  Created by Piotr Sarna on 05.06.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit

class PixelConstraint: NSLayoutConstraint {

    override var constant: CGFloat {
        get {
            return super.constant / UIScreen.main.scale
        }
        set {
            super.constant = newValue
        }
    }
}
