//
//  OptionPickerCellView.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 13/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit

class OptionPickerCellView: UITableViewCell {
    
    var name: String? {
        didSet {
            nameLabel.text = name
        }
    }
    
    var tickAlpha: CGFloat? {
        didSet {
            tickImageView.alpha = tickAlpha ?? 1.0
        }
    }
    
    @IBOutlet private weak var tickImageView: UIImageView!
    @IBOutlet private weak var nameLabel: UILabel!
}
