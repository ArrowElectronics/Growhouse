//
//  DeviceConfigurationViewController.swift
//  ble_mesh-app
//
//  Created by Lukasz Rzepka on 20.04.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import RxSwift
import RxCocoa

class DeviceConfigurationViewController: UIViewController {
    
    var viewModel: DeviceConfigurationViewModel!
    
    private var tabbar: UITabBarController!
    private let disposeBag = DisposeBag()
    
    @IBAction private func changeTab(_ sender: UISegmentedControl) {
        tabbar.selectedIndex = sender.selectedSegmentIndex
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        viewModel.isConnected.asDriver().drive(onNext: { [weak self] (connected) in
            if !connected {
                self?.navigationController?.popViewController(animated: true)
            }
        }).disposed(by: disposeBag)
    }

    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        switch segue.identifier {
        case Storyboard.DeviceConfiguration.Segue.configurationTabbar:
            tabbar = (segue.destination as! UITabBarController)
            let deviceConfigurationDetail = tabbar.viewControllers!.first as! DeviceConfigurationDetailTableViewController
            deviceConfigurationDetail.viewModel = viewModel.deviceConfigurationDetailViewModel
            let deviceInfo = tabbar.viewControllers![1] as! DeviceInfoTableViewController
            deviceInfo.viewModel = DeviceInfoViewModel(viewModel.services, device: viewModel.deviceConfigurationDetailViewModel.device)
        default:
            break;
        }
    }
}
