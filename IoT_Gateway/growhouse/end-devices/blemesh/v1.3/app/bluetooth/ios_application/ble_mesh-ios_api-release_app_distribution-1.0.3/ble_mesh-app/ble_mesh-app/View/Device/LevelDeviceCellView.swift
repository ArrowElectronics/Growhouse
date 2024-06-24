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
            nameLabel.text = viewModel?.device.name
            indexLabel.text = viewModel?.index
            slider.isEnabled = viewModel!.isConnected
            updateUI(with: viewModel!.percentage.value)
        }
    }
    
    @IBOutlet private weak var slider: UISlider!
    @IBOutlet private weak var nameLabel: UILabel!
    @IBOutlet private weak var indexLabel: UILabel!
    @IBOutlet private weak var percentageLabel: UILabel!
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
            self?.updateUI(with: Float(value))
        }
    }
    
    override func hitTest(_ point: CGPoint, with event: UIEvent?) -> UIView? {
        let view = super.hitTest(point, with: event)
        
        updateSWScrollViewForHit(inView: view)
        
        return view
    }
    
    @IBAction private func sliderValueChanged(_ sender: UISlider) {
        viewModel?.observablePercentage.value = sender.value
        percentageLabel.text = "\(Int(sender.value * 100))%"
    }
    
    func updateUI(with percentage: Float) {
        percentageLabel.text = "\(Int(percentage * 100))%"
        slider.value = percentage
    }
}
