//
//  DeviceInfoElementCellView.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 25.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import BluetoothMesh

class DeviceInfoModelCellView: UITableViewCell {
    
    @IBOutlet private weak var elementLabel: UILabel!
    @IBOutlet private weak var vendorLabel: UILabel!
    @IBOutlet private weak var idLabel: UILabel!
    @IBOutlet private weak var descriptionLabel: UILabel!
    
    var model: SBMModel? {
        didSet {
            updateUI()
        }
    }
    
    var elementIndex: Int? {
        didSet {
            elementLabel.text = elementIndex != nil ? "\(elementIndex!)" : ""
        }
    }
    
    private func updateUI() {
        guard let model = model else { return }
        let identifier = String(format: "%04X", model.identifier)
        vendorLabel.text = model.isSIGModel ? "SIG" : "0x" + identifier.suffix(4)
        idLabel.text = "0x" + identifier.prefix(4)
        descriptionLabel.text = model.name
    }
}
