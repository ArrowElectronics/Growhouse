//
//  NetworkDetailViewController.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 19/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import SVProgressHUD

class NetworkDetailViewController: UIViewController {
    
    var viewModel : NetworkDetailViewModel!
    
    private var tabbar: UITabBarController?
    private let barButton = UIButton(type: .custom)

    override func viewDidLoad() {
        super.viewDidLoad()
        
        barButton.addTarget(self, action: #selector(connect), for: .touchUpInside)
        navigationItem.rightBarButtonItems = [UIBarButtonItem(customView: barButton)]
        title = viewModel.subnet.name
        bindConnectedStatus()
        connect()
    }
    
    private func bindConnectedStatus() {
        _ = viewModel.isConnected.asDriver().drive(onNext: { [weak self] connected in
            self?.barButton.setImage(connected ? #imageLiteral(resourceName: "ic_status_GREEN") : #imageLiteral(resourceName: "ic_status_RED"), for: .normal)
            self?.navigationItem.rightBarButtonItems?.last?.customView = self?.barButton
        })
    }
    
    @objc private func connect() {
        if viewModel.isConnected.value {
            viewModel.disconnect()
        } else {
            SVProgressHUD.show(withStatus: "Connecting...")
            viewModel.connect() { success, status in
                success ? SVProgressHUD.showSuccess(withStatus: status) : SVProgressHUD.showError(withStatus: status)
                SVProgressHUD.dismiss(withDelay: 1.5)
            }
        }
    }
    
    @IBAction private func changeTab(_ sender: UISegmentedControl) {
        tabbar!.selectedIndex = sender.selectedSegmentIndex
        
        if (sender.selectedSegmentIndex == 1) {
            (tabbar!.viewControllers![1] as! ProvisionedDevicesViewController).tableView.reloadData()
        }
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        switch segue.identifier {
        case Storyboard.Network.Segue.tabbar?:
            tabbar = (segue.destination as! UITabBarController)
            
            let provisionedDevices = tabbar!.viewControllers![1] as! ProvisionedDevicesViewController
            provisionedDevices.viewModel = viewModel.provisionedDevicesViewModel
            
            let groups = tabbar!.viewControllers!.first as! GroupsViewController
            groups.viewModel = viewModel.groupsViewModel
        default:
            break;
        }
    }
}
