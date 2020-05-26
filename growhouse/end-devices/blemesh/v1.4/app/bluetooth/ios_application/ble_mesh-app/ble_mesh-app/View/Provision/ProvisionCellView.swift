//
//  ProvisionCellView.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 12/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import RxCocoa
import RxSwift

class ProvisionCellView: UITableViewCell {
    
    var viewModel: ProvisionCellViewModel? {
        didSet {
            deviceNameLabel.text = viewModel?.name
            deviceMacAddressLabel.text = viewModel?.macAddress
            viewModel?.signalPower.asObservable().bind(to: signalPowerLabel.rx.text).disposed(by: disposeBag)
        }
    }
    
    var provisionButtonTag: Int? {
        didSet {
            provisionButton.tag = provisionButtonTag ?? 0
        }
    }
    
    private let disposeBag = DisposeBag()
    
    @IBOutlet private weak var deviceNameLabel: UILabel!
    @IBOutlet private weak var deviceMacAddressLabel: UILabel!
    @IBOutlet private weak var signalPowerLabel: UILabel!
    @IBOutlet private weak var provisionButton: UIButton!
    
    override func awakeFromNib() {
        super.awakeFromNib()
        
        provisionButton.layer.borderWidth = 1
        provisionButton.layer.borderColor = UIColor.white.cgColor
        provisionButton.layer.cornerRadius = 4.0
    }
}
