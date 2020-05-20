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
            removeOldObservers()
            bindUIWithViewModel()
            nameLabel.text = viewModel?.device.name
            indexLabel.text = viewModel?.index
            enableInteraction(state: viewModel!.isConnected.value)
        }
    }
    
    @IBOutlet private weak var nameLabel: UILabel!
    @IBOutlet private weak var indexLabel: UILabel!
    @IBOutlet private weak var switchButton: UIButton!
    @IBOutlet private weak var reloadButton: UIButton!
    @IBOutlet weak var separatorLine: UIView!
    
    private var disposableSwitch: Disposable?
    
    deinit {
        removeOldObservers()
    }
    
    private func removeOldObservers() {
        disposableSwitch?.dispose()
    }
    
    private func updateValuesForCell() {
        viewModel?.getValuesFromNetwork()
    }
    
    private func bindUIWithViewModel() {
        disposableSwitch = viewModel?.changeFromNetworkSwitchState.asDriver().drive(onNext: { [weak self] isOn in
            guard isOn.error == nil else {
                self?.setNotKnownOnOffState()
                self?.reloadButton.stopRotation()
                self?.enableInteraction(state: true)
                return
            }
            self?.setSwitchAppearance(for: isOn.value)
            self?.reloadButton.stopRotation()
            self?.enableInteraction(state: true)
        })
    }
    
    @IBAction private func switchToggled(_ sender: UIButton) {
        viewModel?.changeSwitchState()
    }
    
    @IBAction private func onReloadTap(_ sender: UIButton) {
        sender.startRotation()
        updateValuesForCell()
        enableInteraction(state: false)
    }
    
    private func setSwitchAppearance(for on: Bool) {
        switchButton.setImage(on ? #imageLiteral(resourceName: "toggle_ON") : #imageLiteral(resourceName: "toggle_OFF"), for: .normal)
    }
    
    private func setNotKnownOnOffState() {
        switchButton.setImage(#imageLiteral(resourceName: "toggle_OFF"), for: .normal)
    }
    
    private func enableInteraction(state: Bool) {
        switchButton.isEnabled = state
        reloadButton.isEnabled = state
        isUserInteractionEnabled = state
    }
}
