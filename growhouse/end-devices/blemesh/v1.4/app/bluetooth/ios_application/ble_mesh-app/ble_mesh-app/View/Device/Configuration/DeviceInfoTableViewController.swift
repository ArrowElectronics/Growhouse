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
        NotificationCenter.default.addObserver(self, selector: #selector(updateUI), name: BluetoothMeshNotifications.NodesUpdated, object: nil)
    }
    
    override func tableView(_ tableView: UITableView, heightForRowAt indexPath: IndexPath) -> CGFloat {
        if indexPath == IndexPath(item: 0, section: 3) {
            let counts = deviceInfoElementsTableViewController.viewModel.elements.value.map { $0.sigModels.count }
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
            elementsViewController.viewModel = DeviceInfoElementsViewModel(viewModel.services, elements: viewModel.device.elements)
            
            deviceInfoElementsTableViewController = elementsViewController
            deviceInfoElementsTableViewController.viewModel.elements.asDriver().drive(onNext: { [weak self] _ in
                self?.tableView.reloadData()
            }).disposed(by: disposeBag)
        }
    }
    
    @objc private func updateUI() {
        nameLabel.text = viewModel.device.name
        unicastAddressLabel.text =  "\(viewModel.device.primaryElementAddress)"
        uuidLabel.text = viewModel.device.uuid.hexEncodedString()
        networkLabel.text = viewModel.device.subnets.first?.name
        appKeyLabel.text = "\(viewModel.device.groups.first?.appKey.rawKey.hexEncodedString() ?? "none")"
        netKeyLabel.text = "\(viewModel.device.subnets.first?.netKey.rawKey.hexEncodedString() ?? "none")"
        devKeyLabel.text = viewModel.device.devKey.rawKey.hexEncodedString()
    }
}
