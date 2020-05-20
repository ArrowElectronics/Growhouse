//
//  ClientDeviceCellView.swift
//  ble_mesh-app
//
//  Created by Lukasz Rzepka on 02/07/2019.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import SWTableViewCell

class ClientDeviceCellView: SWTableViewCell, SeparatorLineProperty {
    
    var viewModel: ClientDeviceViewModel? { didSet { updateUI() } }
    
    @IBOutlet private weak var nameLabel: UILabel!
    @IBOutlet private weak var indexLabel: UILabel!
    @IBOutlet private weak var functionalityImage: UIImageView!
    @IBOutlet weak var separatorLine: UIView!
    
    private func updateUI() {
        nameLabel.text = viewModel?.device.name
        indexLabel.text = viewModel?.index
        functionalityImage.image = viewModel?.functionalityImage
    }
}
