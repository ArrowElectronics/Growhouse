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
    var elements = [SBMElement]()
    let disposeBag = DisposeBag()
    
    override func viewDidLoad() {
        viewModel.elements.asDriver().drive(onNext: { [weak self] elements in
            self?.elements = elements
            self?.tableView.reloadData()
        }).disposed(by: disposeBag)
    }
    
    override func numberOfSections(in tableView: UITableView) -> Int {
        return elements.count
    }
    
    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return elements[section].models.count
    }
    
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: Storyboard.DeviceConfiguration.CellIdentifiers.elementCell) as! DeviceInfoModelCellView
        
        cell.model = elements[indexPath.section].models[indexPath.row]
        cell.elementIndex = elements[indexPath.section].index
        
        return cell
    }
}
