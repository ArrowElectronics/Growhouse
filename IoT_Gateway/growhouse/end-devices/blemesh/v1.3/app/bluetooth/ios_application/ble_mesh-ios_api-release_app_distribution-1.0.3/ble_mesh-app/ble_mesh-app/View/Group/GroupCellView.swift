//
//  GroupCellView.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 26/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import SWTableViewCell

class GroupCellView: SWTableViewCell, SeparatorLineProperty {
    
    @IBOutlet private weak var nameLabel: UILabel!
    @IBOutlet private weak var infoLabel: UILabel!
    @IBOutlet weak var separatorLine: UIView!
    
    var viewModel: GroupCellViewModel! {
        didSet {
            nameLabel.text = viewModel.name
            infoLabel.text = viewModel.info
        }
    }
}
