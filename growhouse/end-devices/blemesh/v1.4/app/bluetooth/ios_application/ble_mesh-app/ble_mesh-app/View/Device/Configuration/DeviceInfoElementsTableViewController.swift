//
//  DeviceInfoElementsTableViewController.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 25.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import BluetoothMesh
import RxSwift
import RxCocoa

class DeviceInfoElementsTableViewController: UITableViewController {
    
    var viewModel: DeviceInfoElementsViewModel!
    let disposeBag = DisposeBag()
    
    override func viewDidLoad() {
        viewModel.elements.asDriver().drive(onNext: { [weak self] elements in
            self?.tableView.reloadData()
        }).disposed(by: disposeBag)
    }
    
    override func numberOfSections(in tableView: UITableView) -> Int {
        return viewModel.elements.value.count
    }
    
    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return viewModel.elements.value[section].sigModels.count
    }
    
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: Storyboard.DeviceConfiguration.Cell.elementCell) as! DeviceInfoModelCellView
        
        cell.model = viewModel.elements.value[indexPath.section].sigModels[indexPath.row]
        cell.elementIndex = viewModel.elements.value[indexPath.section].address
        
        return cell
    }
}
