//
//  ClientDeviceCellView.swift
//  ble_mesh-app
//
//  Created by Lukasz Rzepka on 02/07/2019.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import SWTableViewCell
import RxSwift
import RxCocoa

class ClientDeviceCellView: SWTableViewCell, SeparatorLineProperty {
    
    var viewModel: ClientDeviceViewModel? {
        didSet {
            nameLabel.text = viewModel?.device.name
            indexLabel.text = viewModel?.index
            setClientFunctionalityImage()
        }
    }
    
    @IBOutlet private weak var nameLabel: UILabel!
    @IBOutlet private weak var indexLabel: UILabel!
    @IBOutlet private weak var functionalityImage: UIImageView!
    @IBOutlet weak var separatorLine: UIView!
    
    private let disposeBag = DisposeBag()
    
    override func didMoveToSuperview() {
        super.didMoveToSuperview()
        
        viewModel?.observableOn.asObservable().subscribe({ [weak self] on in
            self?.setClientFunctionalityImage()
        }).disposed(by: disposeBag)
    }
    
    func setClientFunctionalityImage() {
        if viewModel?.device.configuration?.activeFunctionality?.functionalityId == .genericOnOffClient {
            functionalityImage.image = #imageLiteral(resourceName: "toggle_ON")
        } else if viewModel?.device.configuration?.activeFunctionality?.functionalityId == .genericPowerOnOffClient {
            functionalityImage.image = #imageLiteral(resourceName: "toggle_ON")
        } else if viewModel?.device.configuration?.activeFunctionality?.functionalityId == .lightLightnessClient {
            functionalityImage.image = #imageLiteral(resourceName: "toggle_level")
        } else if viewModel?.device.configuration?.activeFunctionality?.functionalityId == .genericLevelClient {
            functionalityImage.image = #imageLiteral(resourceName: "toggle_level")
        }
    }
}
