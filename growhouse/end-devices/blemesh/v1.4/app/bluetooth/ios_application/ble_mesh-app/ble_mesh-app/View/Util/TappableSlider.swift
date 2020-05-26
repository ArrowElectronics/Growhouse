//
//  TappableSlider.swift
//  ble_mesh-app
//
//  Created by Piotr Sarna on 05.06.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit

class TappableSlider: UISlider {
    
    override func beginTracking(_ touch: UITouch, with event: UIEvent?) -> Bool {
        return true
    }
}
