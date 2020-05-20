//
//  LevelDeviceCellView.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 24.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import SWTableViewCell
import RxSwift
import RxCocoa

class LevelDeviceCellView: SWTableViewCell, UISliderEnhancementsForSWTableViewCell, SeparatorLineProperty {
    var viewModel: LevelDeviceViewModel? {
        didSet {
            removeOldObservers()
            bindUIWithViewModel()
            nameLabel.text = viewModel?.device.name
            indexLabel.text = viewModel?.index
            enableInteraction(state: viewModel!.isConnected.value)
        }
    }
    
    @IBOutlet private weak var slider: UISlider!
    @IBOutlet private weak var nameLabel: UILabel!
    @IBOutlet private weak var indexLabel: UILabel!
    @IBOutlet private weak var percentageLabel: UILabel!
    @IBOutlet private weak var reloadButton: UIButton!
    @IBOutlet weak var separatorLine: UIView!
    weak var swScrollView: UIScrollView!
    
    private var disposableLevel: Disposable?
    
    deinit {
        removeOldObservers()
    }
    
    private func removeOldObservers() {
        disposableLevel?.dispose()
    }
    
    override func didMoveToSuperview() {
        super.didMoveToSuperview()
        configureSWScrollView()
    }
    
    private func bindUIWithViewModel() {
        disposableLevel = viewModel?.changeFromNetworkPercentage.asDriver().drive(onNext: { [weak self] (percentage) in
            guard percentage.error == nil else {
                self?.setNotKnownLevelState()
                self?.reloadButton.stopRotation()
                self?.enableInteraction(state: true)
                return
            }
            self?.updateUI(with: percentage.value)
            self?.reloadButton.stopRotation()
            self?.enableInteraction(state: true)
        })
    }
    
    @IBAction private func sliderValueChanged(_ sender: UISlider) {
        viewModel?.setLevel(value: sender.value)
    }
    
    @IBAction func onReloadTap(_ sender: UIButton) {
        sender.startRotation()
        updateValuesForCell()
        enableInteraction(state: false)
    }
    
    private func updateValuesForCell() {
        viewModel?.getValuesFromNetwork()
    }
    
    private func updateUI(with percentage: Float) {
        if percentage.isNaN {
            setNotKnownLevelState()
            return
        }
        
        percentageLabel.text = "\(Int(percentage * 100))%"
        slider.value = percentage
    }
    
    private func setNotKnownLevelState() {
        slider.setValue(0, animated: false)
        percentageLabel.text = "---"
    }
    
    private func enableInteraction(state: Bool) {
        slider.isEnabled = state
        reloadButton.isEnabled = state
        isUserInteractionEnabled = state
    }
    
    override func hitTest(_ point: CGPoint, with event: UIEvent?) -> UIView? {
        let view = super.hitTest(point, with: event)
        
        updateSWScrollViewForHit(inView: view)
        
        return view
    }
}
