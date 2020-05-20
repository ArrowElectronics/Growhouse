//
//  ProvisionedDevicesViewController.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 20/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import RxSwift
import RxCocoa
import SWTableViewCell
import SVProgressHUD
import BluetoothMesh

class ProvisionedDevicesViewController: UIViewController, UITableViewDelegate, UITableViewDataSource, SWTableViewCellDelegate {
    typealias SWTableViewCellWithSeparatorLine = SWTableViewCell & SeparatorLineProperty
    
    var viewModel : ProvisionedDevicesViewModel!
    
    @IBOutlet private weak var emptyMessage: UIView!
    @IBOutlet private var separatorViews: [UIView]!
    @IBOutlet weak var tableView: UITableView!
    
    private var devices = [ProvisionedDevicesCellViewModel]()
    private var functionalityViewModels = [BaseViewModel?]()
    private var deviceConfigurationViewModel: DeviceConfigurationViewModel?
    private let disposeBag = DisposeBag()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        tableView.delegate = self
        tableView.dataSource = self
        tableView.rowHeight = UITableViewAutomaticDimension
        tableView.estimatedRowHeight = 84
        
        viewModel.devices.asDriver().drive(onNext: { [weak self] (devices) in
            self?.refresh(devices: devices)
            
            for separatorView in self?.separatorViews ?? [] {
                separatorView.isHidden = devices.count == 0
            }
            
            self?.emptyMessage.isHidden = devices.count != 0
            self?.tableView.reloadData()
        }).disposed(by: disposeBag)
        
        viewModel.isConnected.asDriver().drive(onNext: { [weak self] (connected) in
            self?.tableView.reloadData()
        }).disposed(by: disposeBag)
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        
        for separatorView in separatorViews {
            let heightConstraint = PixelConstraint(item: separatorView,
                                                   attribute: .height,
                                                   relatedBy: .equal,
                                                   toItem: nil,
                                                   attribute: .notAnAttribute,
                                                   multiplier: 1,
                                                   constant: 1)
            separatorView.frame.size.height = heightConstraint.constant
        }
    }
    
    private func refresh(devices: [ProvisionedDevicesCellViewModel]) {
        self.devices = devices
        functionalityViewModels = viewModel.getFunctionalityViewModels()
    }
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return devices.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        var cell: SWTableViewCellWithSeparatorLine! = nil
        let device = devices[indexPath.row]
        
        if let functionalityRawValue = viewModel.services.bluetoothMeshNodeFunctionality.activeFunctionality(for: device.node),
            let functionality = SBMModelIdentifier(rawValue: functionalityRawValue) {
            
            switch functionality {
            case .genericOnOffServer,
                 .genericPowerOnOffServer:
                cell = prepareCellForGenericOnOffServer(forIndexPath: indexPath)
            case .genericLevelServer:
                cell = prepareCellForGenericLevelServer(forIndexPath: indexPath)
            case .lightLightnessServer:
                cell = prepareCellForLightLightnessServer(forIndexPath: indexPath)
            case .lightCTLServer:
                cell = prepareCellForCtlServer(forIndexPath: indexPath)
            case .genericOnOffClient,
                 .genericPowerOnOffClient,
                 .lightLightnessClient,
                 .lightCTLClient,
                 .genericLevelClient:
                cell = prepareCellForClientDevice(forIndexPath: indexPath)
            default:
                break
            }
        }
     
        if cell == nil {
            cell = prepareCellForUnknownDevice(forIndexPath: indexPath)
        }
        
        cell.separatorLine.isHidden = indexPath.row == (devices.count - 1)
        cell.rightUtilityButtons = [UIButton.deleteButton]
        cell.delegate = self
        
        return cell
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        tableView.deselectRow(at: indexPath, animated: true)
        
        if viewModel.isConnected.value {
            deviceConfigurationViewModel = DeviceConfigurationViewModel((viewModel?.services)!, device: devices[indexPath.row].node)
            performSegue(withIdentifier: Storyboard.Device.Segue.deviceConfiguration, sender: deviceConfigurationViewModel)
        } else {
            SVProgressHUD.showError(withStatus: "You are not connected to network")
            SVProgressHUD.dismiss(withDelay: 1.5)
        }
    }
    
    private func prepareCellForGenericOnOffServer(forIndexPath indexPath: IndexPath) -> SWTableViewCellWithSeparatorLine {
        let cell = tableView.dequeueReusableCell(withIdentifier: Storyboard.Device.Cell.switchDeviceCell) as! SwitchDeviceCellView
        
        cell.viewModel = functionalityViewModels[indexPath.row] as? SwitchDeviceViewModel
        
        return cell
    }
    
    private func prepareCellForLightLightnessServer(forIndexPath indexPath: IndexPath) -> SWTableViewCellWithSeparatorLine {
        let cell = tableView.dequeueReusableCell(withIdentifier: Storyboard.Device.Cell.lightDeviceCell) as! LightDeviceCellView
        let vm = functionalityViewModels[indexPath.row] as! LightDeviceViewModel
        
        cell.viewModel = vm
        
        return cell
    }
    
    private func prepareCellForCtlServer(forIndexPath indexPath: IndexPath) -> SWTableViewCellWithSeparatorLine {
        let cell = tableView.dequeueReusableCell(withIdentifier: Storyboard.Device.Cell.ctlDeviceCell) as! CtlDeviceCellView
        let vm = functionalityViewModels[indexPath.row] as! CtlDeviceViewModel
        
        cell.viewModel = vm
        
        return cell
    }
    
    private func prepareCellForGenericLevelServer(forIndexPath indexPath: IndexPath) -> SWTableViewCellWithSeparatorLine {
        let cell = tableView.dequeueReusableCell(withIdentifier: Storyboard.Device.Cell.levelDeviceCell) as! LevelDeviceCellView
        let vm = functionalityViewModels[indexPath.row] as! LevelDeviceViewModel
        
        cell.viewModel = vm
        
        return cell
    }
    
    private func prepareCellForUnknownDevice(forIndexPath indexPath: IndexPath) -> SWTableViewCellWithSeparatorLine {
        let cell = tableView.dequeueReusableCell(withIdentifier: Storyboard.Device.Cell.unknownDeviceCell) as! UnknownDeviceCellView
        let vm = functionalityViewModels[indexPath.row] as! UnknownDeviceViewModel
        
        cell.viewModel = vm
        
        return cell
    }
    
    private func prepareCellForClientDevice(forIndexPath indexPath: IndexPath) -> SWTableViewCellWithSeparatorLine {
        let cell = tableView.dequeueReusableCell(withIdentifier: Storyboard.Device.Cell.clientDeviceCell) as! ClientDeviceCellView
        let vm = functionalityViewModels[indexPath.row] as! ClientDeviceViewModel
        
        cell.viewModel = vm
        
        return cell
    }

    func swipeableTableViewCell(_ cell: SWTableViewCell!, didTriggerRightUtilityButtonWith index: Int) {
        guard index == 0 else { return }
        
        let row = tableView.indexPath(for: cell)!.row
        let node = devices[row].node
        
        confirmDelete(node: node, connected: node.isConnectedAsProxy(), selectedCell: cell, row: row)
    }
    
    private func confirmDelete(node: SBMNode, connected: Bool, selectedCell: SWTableViewCell, row: Int) {
        let title = "Delete node"
        var message = "Are you sure you want to delete the selected node?"
        if connected {
            message = "Are you sure you want to delete the selected node? With this node you are connected to Network. After remove node connection with the Network will be abort."
        }
        let alert = UIAlertController(title: title, message: message) { [weak self] ok in
            ok ? self?.handleDeletion(node: node, row: row) : selectedCell.hideUtilityButtons(animated: true)
        }
        present(alert, animated: true, completion: nil)
    }
    
    private func handleDeletion(node: SBMNode, row: Int) {
        SVProgressHUD.show(withStatus: "Removing device...")
        viewModel.remove(device: node) { [weak self] node, error in
            self?.showPopup(for: node, error: error)
            self?.viewModel.updateValues(for: row)
        }
    }
    
    private func showPopup(for node: SBMNode, error: Error?) {
        if error != nil {
            let title = "Could not remove device: \(node.name)"
            let message = "Do you want to force-remove the selected device anyway?"
            let alert = UIAlertController(title: title, message: message, cancelTitle: "No", destructiveTitle: "Yes") { [weak self] ok in
                if ok { self?.viewModel.forceRemove(device: node) }
            }
            present(alert, animated: true, completion: nil)
            SVProgressHUD.dismiss()
        } else {
            SVProgressHUD.showSuccess(withStatus: "Node removed")
            SVProgressHUD.dismiss(withDelay: 1.5)
        }
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        switch segue.identifier {
        case Storyboard.Device.Segue.deviceConfiguration:
            let viewController = segue.destination as! DeviceConfigurationViewController
            viewController.viewModel = (sender as! DeviceConfigurationViewModel)
        default:
            break
        }
    }
}
