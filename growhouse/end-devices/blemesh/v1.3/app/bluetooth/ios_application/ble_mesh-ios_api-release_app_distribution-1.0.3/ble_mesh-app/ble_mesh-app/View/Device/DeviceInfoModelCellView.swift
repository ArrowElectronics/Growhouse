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
            guard let index = elementIndex else {
                elementLabel.text = ""
                return
            }
            
            elementLabel.text = String(index)
        }
    }
    
    private func updateUI() {
        let identifier = String(format: "%04X", model!.identifier.rawValue)
        
        vendorLabel.text = model!.isSigModel ? "SIG" : "0x" + identifier.suffix(4)
        idLabel.text = "0x" + identifier.prefix(4)
        descriptionLabel.text = model?.name
    }
}
