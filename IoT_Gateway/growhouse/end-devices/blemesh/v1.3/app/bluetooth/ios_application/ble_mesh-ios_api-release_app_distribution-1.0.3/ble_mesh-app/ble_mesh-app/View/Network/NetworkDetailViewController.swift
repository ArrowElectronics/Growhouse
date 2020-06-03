//
//  NetworkDetailViewController.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 19/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit

class NetworkDetailViewController: UIViewController {
    
    var viewModel : NetworkDetailViewModel!
    
    private var tabbar: UITabBarController?

    override func viewDidLoad() {
        super.viewDidLoad()
        
        title = viewModel.network.name
        bindConnectedStatus()
    }
    
    private func bindConnectedStatus() {
        _ = viewModel.isConnected.asObservable().subscribe(onNext: { [weak self] (connected) in
            let button = UIButton(type: UIButtonType.custom)
            button.setImage(connected ? #imageLiteral(resourceName: "ic_status_GREEN") : #imageLiteral(resourceName: "ic_status_RED"), for: .normal)
            button.addTarget(self, action: #selector(self?.connect), for: .touchUpInside)
            
            if self?.navigationItem.rightBarButtonItems == nil {
                let barButton = UIBarButtonItem(customView: button)
                self?.navigationItem.rightBarButtonItems = [barButton]
            } else {
                self?.navigationItem.rightBarButtonItems?.last?.customView = button
            }
            
            if let provisionedDevicesViewController = self?.tabbar?.viewControllers?[1] as? ProvisionedDevicesViewController {
                provisionedDevicesViewController.viewModel.isConnected.value = connected
            }
            
            if let groupsViewController = self?.tabbar?.viewControllers?[0] as? GroupsViewController {
                groupsViewController.viewModel.isConnected = connected
            }
        })
    }
    
    @objc private func connect() {
        if viewModel.isConnected.value {
            viewModel.disconnect()
        } else {
            viewModel.connect()
        }
    }
    
    @IBAction private func changeTab(_ sender: UISegmentedControl) {
        tabbar!.selectedIndex = sender.selectedSegmentIndex
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
