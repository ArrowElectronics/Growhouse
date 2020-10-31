//
//  UnknownDeviceCellView.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 20/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import SWTableViewCell

class UnknownDeviceCellView: SWTableViewCell, SeparatorLineProperty {
    
    var viewModel: UnknownDeviceViewModel? {
        didSet {
            nameLabel.text = viewModel?.name
            indexLabel.text = viewModel?.index
        }
    }
    
    @IBOutlet private weak var nameLabel: UILabel!
    @IBOutlet private weak var indexLabel: UILabel!
    @IBOutlet weak var separatorLine: UIView!
}
