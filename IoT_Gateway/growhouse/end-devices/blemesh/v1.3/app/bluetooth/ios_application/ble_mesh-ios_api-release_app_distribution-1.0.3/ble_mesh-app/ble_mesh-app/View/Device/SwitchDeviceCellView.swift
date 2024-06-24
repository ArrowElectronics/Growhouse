//
//  SwitchDeviceCellView.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 20/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import SWTableViewCell
import RxSwift
import RxCocoa

class SwitchDeviceCellView: SWTableViewCell, SeparatorLineProperty {
    
    var viewModel: SwitchDeviceViewModel? {
        didSet {
            nameLabel.text = viewModel?.device.name
            indexLabel.text = viewModel?.index
            switchButton.isEnabled = viewModel!.isConnected
        }
    }
    
    @IBOutlet private weak var nameLabel: UILabel!
    @IBOutlet private weak var indexLabel: UILabel!
    @IBOutlet private weak var switchButton: UIButton!
    @IBOutlet weak var separatorLine: UIView!
    
    private let disposeBag = DisposeBag()
    
    @IBAction private func switchToggled(_ sender: UIButton) {
        viewModel?.observableOn.value = (viewModel?.observableOn.value)! ? false : true
        setSwitchAppearance(for: viewModel!.observableOn.value)
    }
    
    override func didMoveToSuperview() {
        super.didMoveToSuperview()
        
        viewModel?.observableOn.asObservable().subscribe(onNext: { [weak self] (on) in
            self?.setSwitchAppearance(for: on)
        }).disposed(by: disposeBag)
        
        NotificationCenter.default.addObserver(forName: MeshNotifications.GroupValueChanged, object: nil, queue: .main) { [weak self] (notification) in
            let value = notification.userInfo?["value"] as! Double
            self?.setSwitchAppearance(for: value > 0.0)
        }
    }
    
    func setSwitchAppearance(for on: Bool) {
        switchButton.setImage(on ? #imageLiteral(resourceName: "toggle_ON") : #imageLiteral(resourceName: "toggle_OFF"), for: .normal)
    }
}
