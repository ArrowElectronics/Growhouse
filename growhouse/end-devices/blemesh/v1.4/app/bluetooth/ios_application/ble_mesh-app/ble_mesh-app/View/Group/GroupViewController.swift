//
//  GroupViewController.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 07.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import RxSwift
import RxCocoa
import UIKit

class GroupViewController: UIViewController {
    
    var viewModel: GroupViewModel!
    var devicesViewModel: ProvisionedDevicesViewModel!
    
    @IBOutlet private var masterControlViews: [UIView]!
    @IBOutlet private var devicesViews: [UIView]!
    @IBOutlet private weak var emptyMessage: UIView!
    
    private let disposeBag = DisposeBag()
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        switch segue.identifier {
        case Storyboard.Group.Segue.showDevices:
            let provisionedDevicesViewModel = ProvisionedDevicesViewModel(viewModel.services, subnet: viewModel.subnet, group: viewModel.group)
            let destinationViewController = segue.destination as! ProvisionedDevicesViewController
            destinationViewController.viewModel = provisionedDevicesViewModel
        case Storyboard.Group.Segue.showMasterControl:
            let groupMasterControlViewModel = GroupMasterControlViewModel(viewModel.services, group: viewModel.group, isOn: false, percentage: 0.0)
            let destinationViewController = segue.destination as! GroupMasterControlViewController
            destinationViewController.viewModel = groupMasterControlViewModel
        default:
            break
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        devicesViewModel.devices.asDriver().drive(onNext: { [weak self] (devices) in
            let shouldShowMasterControl = devices.count >= 2
            let shouldShowDevices = devices.count > 0
            
            for masterControlView in self?.masterControlViews ?? [] {
                masterControlView.isHidden = !shouldShowMasterControl
            }
            
            for devicesView in self?.devicesViews ?? [] {
                devicesView.isHidden = !shouldShowDevices
            }
            
            self?.emptyMessage.isHidden = shouldShowDevices
        }).disposed(by: disposeBag)
        
    }
    
}
