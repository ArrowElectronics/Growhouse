//
//  NetworkCellView.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 12/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import SWTableViewCell

class NetworkCellView: SWTableViewCell {
    
    var name: String? {
        didSet {
            nameLabel.text = name
        }
    }
    
    var info: String? {
        didSet {
            infoLabel.text = info
        }
    }
    
    @IBOutlet private weak var nameLabel: UILabel!
    @IBOutlet private weak var infoLabel: UILabel!
}
