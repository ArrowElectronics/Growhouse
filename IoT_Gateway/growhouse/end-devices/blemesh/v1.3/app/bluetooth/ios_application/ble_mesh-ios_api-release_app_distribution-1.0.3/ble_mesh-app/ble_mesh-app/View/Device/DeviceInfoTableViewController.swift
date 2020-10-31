//
//  DeviceInfoTableViewController.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 25.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import BluetoothMesh
import RxSwift

class DeviceInfoTableViewController: UITableViewController {
    
    var viewModel: DeviceInfoViewModel!

    @IBOutlet private weak var nameLabel: UILabel!
    @IBOutlet private weak var unicastAddressLabel: UILabel!
    @IBOutlet private weak var uuidLabel: UILabel!
    @IBOutlet private weak var networkLabel: UILabel!
    @IBOutlet private weak var netKeyLabel: UILabel!
    @IBOutlet private weak var appKeyLabel: UILabel!
    @IBOutlet private weak var devKeyLabel: UILabel!
    let disposeBag = DisposeBag()
    
    private weak var deviceInfoElementsTableViewController: DeviceInfoElementsTableViewController!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        updateUI()
        NotificationCenter.default.addObserver(self, selector: #selector(updateUI), name: MeshNotifications.NodesUpdated, object: nil)
        
        tableView.rowHeight = UITableViewAutomaticDimension
        tableView.estimatedRowHeight = 60
    }
    
    override func tableView(_ tableView: UITableView, heightForRowAt indexPath: IndexPath) -> CGFloat {
        if indexPath.section == 3, indexPath.row == 0 {
            let counts = deviceInfoElementsTableViewController.elements.map { (element) -> Int in
                return element.models.count
            }
            let elementsCount = counts.reduce(0, +)
            let rowHeight = deviceInfoElementsTableViewController.tableView.rowHeight
            let allRowsHeight = rowHeight * CGFloat(elementsCount)
            let headerHeight = CGFloat(44)
            
            return headerHeight + allRowsHeight
        }
    
        return super.tableView(tableView, heightForRowAt: indexPath)
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        if segue.identifier! == Storyboard.DeviceConfiguration.Segue.elements {
            let elementsViewController = segue.destination as! DeviceInfoElementsTableViewController
            elementsViewController.viewModel = DeviceInfoElementsViewModel(viewModel.services, node: self.viewModel.device)
            
            deviceInfoElementsTableViewController = elementsViewController
            deviceInfoElementsTableViewController.viewModel.elements.asDriver().drive(onNext: { [weak self] models in
                self?.tableView.reloadData()
            }).disposed(by: disposeBag)
        }
    }
    
    @objc private func updateUI() {
        let network = viewModel.services.networkManager.network(forNetworkIndex: viewModel.device.networkIndex)
        let group = viewModel.services.networkManager.group(withMeshNode: viewModel.device)
        
        nameLabel.text = viewModel.device.name
        unicastAddressLabel.text =  "\(viewModel.device.meshAddress)"
        uuidLabel.text = viewModel.device.meshDeviceUUID.hexEncodedString()
        networkLabel.text = network?.name
        appKeyLabel.text = "\(group?.applicationKey.hexEncodedString() ?? "none")"
        netKeyLabel.text = "\(network?.key.hexEncodedString() ?? "none")"
        devKeyLabel.text = viewModel.services.bluetoothMeshService.bleMeshManager.deviceKey(forUUID: viewModel.device.meshDeviceUUID)?.hexEncodedString()
    }
}
