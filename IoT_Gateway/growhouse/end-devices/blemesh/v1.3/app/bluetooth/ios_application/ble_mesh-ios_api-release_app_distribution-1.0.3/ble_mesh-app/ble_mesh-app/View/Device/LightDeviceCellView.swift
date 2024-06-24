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
            nameLabel.text = viewModel?.device.name
            indexLabel.text = viewModel?.index
            slider.isEnabled = viewModel!.isConnected
            lightButton.isEnabled = viewModel!.isConnected
            
            updateUI(with: viewModel!.percentage.value)
        }
    }
    
    @IBOutlet private weak var lampImageView: UIImageView!
    @IBOutlet private weak var nameLabel: UILabel!
    @IBOutlet private weak var indexLabel: UILabel!
    @IBOutlet private weak var percentageLabel: UILabel!
    @IBOutlet private weak var slider: UISlider!
    @IBOutlet private weak var lightButton: UIButton!
    @IBOutlet weak var separatorLine: UIView!
    weak var swScrollView: UIScrollView!
    
    private let disposeBag = DisposeBag()
    
    override func didMoveToSuperview() {
        super.didMoveToSuperview()
        
        configureSWScrollView()
        viewModel?.percentage.asObservable().subscribe(onNext: { [weak self] (percentage) in
            self?.updateUI(with: percentage)
        }).disposed(by: disposeBag)
        
        NotificationCenter.default.addObserver(forName: MeshNotifications.GroupValueChanged, object: nil, queue: .main) { [weak self] (notification) in
            let value = notification.userInfo?["value"] as! Double
            self?.updateUI(with: value)
        }
    }
    
    override func hitTest(_ point: CGPoint, with event: UIEvent?) -> UIView? {
        let view = super.hitTest(point, with: event)
        
        updateSWScrollViewForHit(inView: view)
        
        return view
    }
    
    @IBAction private func sliderValueChanged(_ sender: UISlider) {
        viewModel?.observablePercentage.value = Double(sender.value)
        percentageLabel.text = "\(Int(viewModel!.observablePercentage.value * 100))%"
        lampImageView.image = viewModel!.observablePercentage.value < 0.01 ? #imageLiteral(resourceName: "lamp_OFF") : #imageLiteral(resourceName: "lamp_ON")
    }
    
    @IBAction func onLightTap(_ sender: UIButton) {
        let newValue = (viewModel?.percentage.value)! < 0.01 ? 1 : 0
        
        viewModel?.observablePercentage.value = Double(newValue)
        updateUI(with: viewModel!.observablePercentage.value)
    }
    
    func updateUI(with percentage: Double) {
        percentageLabel.text = "\(Int(percentage * 100))%"
        lampImageView.image = percentage < 0.01 ? #imageLiteral(resourceName: "lamp_OFF") : #imageLiteral(resourceName: "lamp_ON")
        slider.value = Float(percentage)
    }
}
