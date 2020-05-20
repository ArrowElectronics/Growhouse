//
//  LightDeviceCellView.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 20/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import SWTableViewCell
import RxSwift
import RxCocoa

class LightDeviceCellView: SWTableViewCell, UISliderEnhancementsForSWTableViewCell, SeparatorLineProperty {
    var viewModel: LightDeviceViewModel? {
        didSet {
            removeOldObservers()
            bindUIWithViewModel()
            nameLabel.text = viewModel?.device.name
            indexLabel.text = viewModel?.index
            enableInteraction(state: viewModel!.isConnected.value)
        }
    }
    
    @IBOutlet private weak var lampImageView: UIImageView!
    @IBOutlet private weak var nameLabel: UILabel!
    @IBOutlet private weak var indexLabel: UILabel!
    @IBOutlet private weak var percentageLabel: UILabel!
    @IBOutlet private weak var slider: UISlider!
    @IBOutlet private weak var lightButton: UIButton!
    @IBOutlet private weak var reloadButton: UIButton!
    @IBOutlet weak var separatorLine: UIView!
    weak var swScrollView: UIScrollView!
    
    private var disposableLightness: Disposable?
    
    deinit {
        removeOldObservers()
    }
    
    private func removeOldObservers() {
        disposableLightness?.dispose()
    }
    
    override func didMoveToSuperview() {
        super.didMoveToSuperview()
        configureSWScrollView()
    }
    
    private func bindUIWithViewModel() {
        disposableLightness = viewModel?.changeFromNetworkLightness.asDriver().drive(onNext: { [weak self] (percentage) in
            guard percentage.error == nil else {
                self?.setNotKnownLightnessState()
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
        viewModel?.setLightness(value: Double(sender.value))
    }
    
    @IBAction private func onLightTap(_ sender: UIButton) {
        viewModel?.changeLightOnOff()
    }
    
    @IBAction func onReloadTap(_ sender: UIButton) {
        enableInteraction(state: false)
        sender.startRotation()
        updateValuesForCell()
    }
    
    private func updateValuesForCell() {
        viewModel?.getValuesFromNetwork()
    }
    
    private func updateUI(with percentage: Double) {
        if percentage.isNaN {
            setNotKnownLightnessState()
            return
        }
        
        percentageLabel.text = "\(Int(percentage * 100))%"
        slider.value = Float(percentage)
        
        if percentage < 0.01 {
            lampImageView.image = #imageLiteral(resourceName: "lamp_OFF")
        } else {
            lampImageView.image = #imageLiteral(resourceName: "lamp_ON")
        }
    }
    
    
    private func setNotKnownLightnessState() {
        slider.setValue(0, animated: false)
        percentageLabel.text = "---"
        lampImageView.image = #imageLiteral(resourceName: "lamp_OFF")
    }
    
    private func enableInteraction(state: Bool) {
        slider.isEnabled = state
        lightButton.isEnabled = state
        reloadButton.isEnabled = state
        isUserInteractionEnabled = state
    }
    
    override func hitTest(_ point: CGPoint, with event: UIEvent?) -> UIView? {
        let view = super.hitTest(point, with: event)
        
        updateSWScrollViewForHit(inView: view)
        
        return view
    }
}
