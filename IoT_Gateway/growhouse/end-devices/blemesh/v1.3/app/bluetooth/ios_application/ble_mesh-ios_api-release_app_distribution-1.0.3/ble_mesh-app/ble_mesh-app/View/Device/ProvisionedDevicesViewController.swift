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
    
    @IBOutlet private weak var tableView: UITableView!
    @IBOutlet private weak var emptyMessage: UILabel!
    @IBOutlet private var separatorViews: [UIView]!
    
    private var devices = [ProvisionedDevicesCellViewModel]()
    private var deviceConfigurationViewModel: DeviceConfigurationViewModel?
    private let disposeBag = DisposeBag()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        tableView.delegate = self
        tableView.dataSource = self
        tableView.rowHeight = UITableViewAutomaticDimension
        tableView.estimatedRowHeight = 84
        
        viewModel.devices.asDriver().drive(onNext: { [weak self] (devices) in
            self?.devices = devices
            
            for separatorView in self?.separatorViews ?? [] {
                separatorView.isHidden = devices.count == 0
            }
            
            self?.emptyMessage.isHidden = devices.count != 0
            self?.tableView.reloadData()
        }).disposed(by: disposeBag)
        
        viewModel.isConnected.asObservable().subscribe(onNext: { [weak self] (connected) in
            self?.deviceConfigurationViewModel?.isConnected.value = connected
            self?.tableView.reloadData()
        }).disposed(by: disposeBag)
        
        NotificationCenter.default.addObserver(forName: MeshNotifications.GroupValueChanged, object: nil, queue: .main) { [weak self] (notification) in
            let value = notification.userInfo?["value"] as! Double
            if let weakSelf = self {
                for idx in 0..<weakSelf.viewModel.devices.value.count {
                    weakSelf.viewModel.values[idx] = NSNumber(value: value)
                }
            }
        }
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
        
        tableView.reloadData()
    }
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return devices.count
    }
    
    func tableView(_ tableView: UITableView, didEndDisplaying cell: UITableViewCell, forRowAt indexPath: IndexPath) {
        guard viewModel.values[indexPath.row] != nil else {
            return
        }
        
        if let switchDeviceCell = cell as? SwitchDeviceCellView {
            viewModel.values[indexPath.row] = NSNumber(value: switchDeviceCell.viewModel!.isOn)
        } else if let lightDeviceCell = cell as? LightDeviceCellView {
            viewModel.values[indexPath.row] = NSNumber(value: lightDeviceCell.viewModel!.percentage.value)
        } else if let levelDeviceCell = cell as? LevelDeviceCellView {
            viewModel.values[indexPath.row] = NSNumber(value: levelDeviceCell.viewModel!.percentage.value)
        }
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        var cell: SWTableViewCellWithSeparatorLine! = nil
        let device = devices[indexPath.row]
        
        if let functionalityRawValue = device.meshNode.configuration?.activeFunctionality?.functionalityId.rawValue,
            let functionality = SBMModelIdentifier(rawValue: functionalityRawValue) {
            
            switch functionality {
            case .genericOnOffClient:
                cell = prepareCellForClientDevice(withDevice: device, withIndexPath: indexPath)
            case .genericOnOffServer:
                cell = prepareCellForGenericOnOffServer(withDevice: device, withIndexPath: indexPath)
            case .genericPowerOnOffClient:
                cell = prepareCellForClientDevice(withDevice: device, withIndexPath: indexPath)
            case .genericPowerOnOffServer:
                cell = prepareCellForGenericOnOffServer(withDevice: device, withIndexPath: indexPath)
            case .lightLightnessClient:
                cell = prepareCellForClientDevice(withDevice: device, withIndexPath: indexPath)
            case .lightLightnessServer:
                cell = prepareCellForLightLightnessServer(withDevice: device, withIndexPath: indexPath)
            case .genericLevelClient:
                cell = prepareCellForClientDevice(withDevice: device, withIndexPath: indexPath)
            case .genericLevelServer:
                cell = prepareCellForGenericLevelServer(withDevice: device, withIndexPath: indexPath)
            default:
                break
            }
        }
     
        if cell == nil {
            cell = prepareCellForUnknownDevice(withDevice: device, withIndexPath: indexPath)
        }
        
        cell.separatorLine.isHidden = indexPath.row == (devices.count - 1)
        cell.rightUtilityButtons = [UIButton.deleteButton]
        cell.delegate = self
        
        return cell
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        tableView.deselectRow(at: indexPath, animated: true)
        
        if viewModel.isConnected.value {
            deviceConfigurationViewModel = DeviceConfigurationViewModel.init((viewModel?.services)!, device: devices[indexPath.row].meshNode)
            performSegue(withIdentifier: Storyboard.Network.Segue.deviceConfiguration, sender: deviceConfigurationViewModel)
        } else {
            SVProgressHUD.showError(withStatus: "You are not connected to network")
            SVProgressHUD.dismiss(withDelay: 1.5)
        }
    }
    
    private func prepareCellForGenericOnOffServer(withDevice device: ProvisionedDevicesCellViewModel, withIndexPath indexPath: IndexPath) -> SWTableViewCellWithSeparatorLine {
        let cell = tableView.dequeueReusableCell(withIdentifier: Storyboard.Network.CellIdentifiers.switchDeviceCell) as! SwitchDeviceCellView
        let vm = SwitchDeviceViewModel(viewModel.services, device: device.meshNode, index: "\(indexPath.row)", isOn: false)
        
        vm.isConnected = viewModel.isConnected.value
        
        if viewModel.values[indexPath.row] == nil {
            vm.stateReceivedCallback = { on in
                self.viewModel.values[indexPath.row] = NSNumber(value: on ? 100.0 : 0.0)
                if let cellToUpdate = self.tableView.cellForRow(at: indexPath) as? SwitchDeviceCellView {
                    cellToUpdate.setSwitchAppearance(for: on)
                }
            }
        } else {
            vm.isOn = viewModel.values[indexPath.row]!.doubleValue > 0.0 ? true : false
        }
        
        cell.viewModel = vm
        
        return cell
    }
    
    private func prepareCellForLightLightnessServer(withDevice device: ProvisionedDevicesCellViewModel, withIndexPath indexPath: IndexPath) -> SWTableViewCellWithSeparatorLine {
        let cell = tableView.dequeueReusableCell(withIdentifier: Storyboard.Network.CellIdentifiers.lightDeviceCell) as! LightDeviceCellView
        let vm = LightDeviceViewModel(viewModel.services, device: device.meshNode, index: "\(indexPath.row)", percentage: 0.0)
        
        vm.isConnected = viewModel.isConnected.value
        
        if viewModel.values[indexPath.row] == nil {
            vm.stateReceivedCallback = { percentage in
                self.viewModel.values[indexPath.row] = NSNumber(value: percentage)
                if let cellToUpdate = self.tableView.cellForRow(at: indexPath) as? LightDeviceCellView {
                    cellToUpdate.updateUI(with: percentage)
                }
            }
        } else {
            vm.percentage.value = viewModel.values[indexPath.row]!.doubleValue
        }
        cell.viewModel = vm
        
        return cell
    }
    
    private func prepareCellForGenericLevelServer(withDevice device: ProvisionedDevicesCellViewModel, withIndexPath indexPath: IndexPath) -> SWTableViewCellWithSeparatorLine {
        let cell = tableView.dequeueReusableCell(withIdentifier: Storyboard.Network.CellIdentifiers.levelDeviceCell) as! LevelDeviceCellView
        let vm = LevelDeviceViewModel(viewModel.services, device: device.meshNode, index: "\(indexPath.row)", percentage: 0.0)
        
        vm.isConnected = viewModel.isConnected.value
        
        if viewModel.values[indexPath.row] == nil {
            vm.stateReceivedCallback = { percentage in
                self.viewModel.values[indexPath.row] = NSNumber(value: percentage)
                if let cellToUpdate = self.tableView.cellForRow(at: indexPath) as? LevelDeviceCellView {
                    cellToUpdate.updateUI(with: percentage)
                }
            }
        } else {
            vm.percentage.value = viewModel.values[indexPath.row]!.floatValue
        }
        
        cell.viewModel = vm
        
        return cell
    }
    
    private func prepareCellForUnknownDevice(withDevice device: ProvisionedDevicesCellViewModel, withIndexPath indexPath: IndexPath) -> SWTableViewCellWithSeparatorLine {
        let cell = tableView.dequeueReusableCell(withIdentifier: Storyboard.Network.CellIdentifiers.unknownDeviceCell) as! UnknownDeviceCellView
        let vm = UnknownDeviceViewModel(viewModel.services, name: device.name!, index: "\(indexPath.row)")
        
        vm.isConnected = viewModel.isConnected.value
        cell.viewModel = vm
        
        return cell
    }
    
    private func prepareCellForClientDevice(withDevice device: ProvisionedDevicesCellViewModel, withIndexPath indexPath: IndexPath) -> SWTableViewCellWithSeparatorLine {
        let cell = tableView.dequeueReusableCell(withIdentifier: Storyboard.Network.CellIdentifiers.clientDeviceCell) as! ClientDeviceCellView
        let vm = ClientDeviceViewModel(viewModel.services, device: device.meshNode, index: "\(indexPath.row)")
        
        vm.isConnected = viewModel.isConnected.value
        cell.viewModel = vm
        
        return cell
    }
    
    func swipeableTableViewCell(_ cell: SWTableViewCell!, canSwipeTo state: SWCellState) -> Bool {
        return viewModel.isConnected.value
    }

    func swipeableTableViewCell(_ cell: SWTableViewCell!, didTriggerRightUtilityButtonWith index: Int) {
        guard index == 0 else { return }
        
        let row = tableView.indexPath(for: cell)!.row
        let meshNode = devices[row].meshNode
        SVProgressHUD.show(withStatus: "Removing device...")
        viewModel.remove(device: meshNode) { [weak self] success in
            if success {
                SVProgressHUD.showSuccess(withStatus: "Device removed")
            } else {
                SVProgressHUD.showError(withStatus: "Could not remove device")
            }
            SVProgressHUD.dismiss(withDelay: 1.5)
            
            self?.viewModel.updateValues(for: row)
        }
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        switch segue.identifier {
        case Storyboard.Network.Segue.deviceConfiguration:
            let viewController = segue.destination as! DeviceConfigurationViewController
            viewController.viewModel = sender as! DeviceConfigurationViewModel
        default:
            break
        }
    }
}
