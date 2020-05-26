//
//  CtlDeviceCellView.swift
//  ble_mesh-app
//
//  Created by Lukasz Rzepka on 25/07/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import SWTableViewCell
import RxSwift
import RxCocoa

class CtlDeviceCellView: SWTableViewCell, UISliderEnhancementsForSWTableViewCell, SeparatorLineProperty {
    var viewModel: CtlDeviceViewModel? {
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
    @IBOutlet private weak var temperatureLabel: UILabel!
    @IBOutlet private weak var percentageLabel: UILabel!
    @IBOutlet private weak var deltaUvLabel: UILabel!
    @IBOutlet private weak var temperatureSlider: UISlider!
    @IBOutlet private weak var lightnessSlider: UISlider!
    @IBOutlet private weak var deltaUvSlider: UISlider!
    @IBOutlet private weak var lightButton: UIButton!
    @IBOutlet private weak var reloadButton: UIButton!
    @IBOutlet weak var separatorLine: UIView!
    weak var swScrollView: UIScrollView!
    
    private var disposableLightness: Disposable?
    private var disposableTemperature: Disposable?
    private var disposableDeltaUV: Disposable?
    
    deinit {
        removeOldObservers()
    }
    
    private func removeOldObservers() {
        disposableLightness?.dispose()
        disposableDeltaUV?.dispose()
        disposableTemperature?.dispose()
    }
    
    override func didMoveToSuperview() {
        super.didMoveToSuperview()
        configureSWScrollView()
    }
    
    private func bindUIWithViewModel() {
        bindUIWithLightness()
        bindUIWithTemperature()
        bindUIWithDeltaUV()
    }
    
    private func bindUIWithLightness() {
        disposableLightness = viewModel?.changeFromNetworkLightnessPercentage.asDriver().drive(onNext: { [weak self] percentage in
            if percentage.error != nil {
                self?.setNotKnownLightnessState()
            } else {
                self?.updateLightnessUI(with: percentage.value)
            }
        })
    }
    
    private func bindUIWithTemperature() {
        disposableTemperature = viewModel?.changeFromNetworkTemperature.asDriver().drive(onNext: { [weak self] temperature in
            if temperature.error != nil {
                self?.setNotKnownTemperatureState()
            } else {
                self?.updateTemperatureUI(with: temperature.value)
            }
        })
    }
    
    private func bindUIWithDeltaUV() {
        disposableDeltaUV = viewModel?.changeFromNetworkDeltaUv.asDriver().drive(onNext: { [weak self] deltaUV in
            if deltaUV.error != nil {
                self?.setNotKnownDeltaUVState()
            } else {
                self?.updateDeltaUvUI(with: deltaUV.value)
            }
        })
    }
    
    @IBAction private func sliderValueChanged(_ sender: UISlider) {
        viewModel?.setLightness(value: Double(sender.value))
    }
    
    @IBAction private func temperatureSliderValueChangerd(_ sender: UISlider) {
        viewModel?.setTemperature(value: Double(sender.value))
    }
    
    @IBAction private func deltaUvSliderValueChanged(_ sender: UISlider) {
        viewModel?.setDelataUV(value: Double(sender.value))
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
        viewModel?.getValuesFromNetwork() { [weak self] in
            self?.reloadButton.stopRotation()
            self?.enableInteraction(state: true)
        }
    }
    
    private func updateLightnessUI(with lightnessPercentage: Double) {
        if lightnessPercentage.isNaN {
            setNotKnownLightnessState()
            return
        }
        
        percentageLabel.text = "\(Int(lightnessPercentage * 100))%"
        lightnessSlider.value = Float(lightnessPercentage)
        
        if lightnessPercentage < 0.01 {
            lampImageView.image = #imageLiteral(resourceName: "lamp_OFF")
        } else {
            lampImageView.image = #imageLiteral(resourceName: "lamp_ON")
        }
    }
    
    private func updateTemperatureUI(with temperatureK: Double) {
         if temperatureK.isNaN {
            setNotKnownTemperatureState()
            return
        }
        
        temperatureLabel.text = "\(String(format: "%.0f", temperatureK))K"
        temperatureSlider.value = Float((temperatureK - 800.0) / (20000.0 - 800.0))
    }
    
    private func updateDeltaUvUI(with deltaUv: Double) {
        if deltaUv.isNaN {
            setNotKnownDeltaUVState()
            return
        }
        
        deltaUvLabel.text = String(format: "%.02f", deltaUv)
        deltaUvSlider.value = Float((deltaUv + 1) / 2.0)
    }
    
    private func setNotKnownTemperatureState() {
        temperatureSlider.setValue(0, animated: false)
        temperatureLabel.text = "---"
    }
    
    private func setNotKnownLightnessState() {
        lightnessSlider.setValue(0, animated: false)
        percentageLabel.text = "---"
        lampImageView.image = #imageLiteral(resourceName: "lamp_OFF")
    }
    
    private func setNotKnownDeltaUVState() {
        deltaUvSlider.setValue(0, animated: false)
        deltaUvLabel.text = "---"
    }
    
    private func enableInteraction(state: Bool) {
        lightnessSlider.isEnabled = state
        temperatureSlider.isEnabled = state
        deltaUvSlider.isEnabled = state
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
