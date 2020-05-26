//
//  GroupMasterControlViewController.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 15.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import RxCocoa
import RxSwift
import UIKit

class GroupMasterControlViewController: UIViewController {
    
    private let disposeBag = DisposeBag()
    var viewModel: GroupMasterControlViewModel? {
        didSet {
            guard let viewModel = viewModel else {
                return
            }
            
            viewModel.observablePercentage.asDriver().drive(onNext: { (value) in
                self.percentageLabel?.text = "\(Int(value * 100))%"
                self.lightSlider?.value = Float(value)
            }).disposed(by: disposeBag)
        }
    }

    @IBOutlet private weak var percentageLabel: UILabel!
    @IBOutlet private weak var lightSlider: UISlider!
    @IBOutlet private weak var switchButton: UIButton!
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        
        let value = Float(viewModel!.observablePercentage.value)
        
        updateUI(for: value)
        lightSlider.value = value
    }
    
    @IBAction private func switchToggled(_ sender: UIButton) {
        viewModel!.change(on: !viewModel!.isOn)
        updateUI(for: viewModel!.isOn ? 1.0 : 0.0)
        lightSlider.value = Float(viewModel!.isOn ? 1.0 : 0.0)
        NotificationCenter.default.post(name: MeshNotifications.GroupValueChanged, object: nil, userInfo: ["value" : viewModel!.isOn ? 1.0 : 0.0])
    }
    
    @IBAction private func sliderValueChanged(_ sender: UISlider) {
        viewModel?.observablePercentage.value = Double(sender.value)
        viewModel!.isOn = sender.value > 0.0
        updateUI(for: sender.value)
        NotificationCenter.default.post(name: MeshNotifications.GroupValueChanged, object: nil, userInfo: ["value" : Double(sender.value)])
    }
    
    private func updateUI(for value: Float) {
        switchButton.setImage(value > 0.0 ? #imageLiteral(resourceName: "toggle_ON") : #imageLiteral(resourceName: "toggle_OFF"), for: .normal)
        percentageLabel.text = "\(Int(value * 100))%"
    }
}
