//
//  DeviceConfigurationDetailTableViewController.swift
//  ble_mesh-app
//
//  Created by Lukasz Rzepka on 20.04.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import RxCocoa
import RxSwift
import BluetoothMesh
import SVProgressHUD

class DeviceConfigurationDetailTableViewController: UITableViewController {
    
    var viewModel: DeviceConfigurationDetailViewModel?
    
    @IBOutlet private weak var name: UILabel!
    @IBOutlet private weak var proxySwitch: UISwitch!
    @IBOutlet private weak var relaySwitch: UISwitch!
    @IBOutlet private weak var friendSwitch: UISwitch!
    @IBOutlet private weak var functionality: UILabel!
    @IBOutlet private weak var group: UILabel!
    @IBOutlet var friendCell: UITableViewCell!
    @IBOutlet var lowPowerCell: UITableViewCell!
    
    private let disposeBag = DisposeBag()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        Observable.from(optional: viewModel?.device.name).bind(to: name.rx.text).disposed(by: disposeBag)
        viewModel?.groupName.asObservable().bind(to: group.rx.text).disposed(by: disposeBag)
        viewModel?.modelFunctionalityName.asObservable().bind(to: functionality.rx.text).disposed(by: disposeBag)
        viewModel?.isProxy.asDriver().drive(onNext: { [weak self] isProxy in
            self?.proxySwitch.isOn = isProxy
        }).disposed(by: disposeBag)
        
        viewModel?.isRelay.asDriver().drive(onNext: { [weak self] isRelay in
            self?.relaySwitch.isOn = isRelay
        }).disposed(by: disposeBag)
        
        viewModel?.supportFriend.asDriver().drive(onNext: { [weak self] friendSupport in
            self?.tableView.reloadData()
        }).disposed(by: disposeBag)
        
        viewModel?.supportLowPower.asDriver().drive(onNext: { [weak self] lowPowerSupport in
            self?.tableView.reloadData()
        }).disposed(by: disposeBag)
        
        viewModel?.isFriend.asDriver().drive(onNext: { [weak self] isFriend in
            self?.friendSwitch.isOn = isFriend
        }).disposed(by: disposeBag)
        
        SVProgressHUD.show(withStatus: "Loading node configuration")
        viewModel?.checkConfiguration() { success in
            if success {
                SVProgressHUD.showSuccess(withStatus: "Node configuration loaded")
            } else {
                SVProgressHUD.showError(withStatus: "Node configuration load failed")
            }
            SVProgressHUD.dismiss(withDelay: 1.5)
        }
    }
    
    override func shouldPerformSegue(withIdentifier identifier: String, sender: Any?) -> Bool {
        if identifier == Storyboard.DeviceConfiguration.Segue.selectFunctionality {
            if viewModel?.device.groups.first != nil { return true }
            
            SVProgressHUD.showError(withStatus: "You need to add device to group")
            SVProgressHUD.dismiss(withDelay: 1.5)
            return false
        }
        
        return true
    }
    
    override func tableView(_ tableView: UITableView, heightForRowAt indexPath: IndexPath) -> CGFloat {
        return indexPath.section == 0 ? heightForCell(at: indexPath) : tableView.rowHeight
    }
    
    private func heightForCell(at indexPath: IndexPath) -> CGFloat {
        if indexPath.row == friendCell.tag {
            return viewModel?.supportFriend.value == true ? tableView.rowHeight : 0
        } else if indexPath.row == lowPowerCell.tag {
            return viewModel?.supportLowPower.value == true ? tableView.rowHeight : 0
        } else {
            return tableView.rowHeight
        }
    }
    
    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        tableView.deselectRow(at: indexPath, animated: true)
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        if segue.identifier == Storyboard.DeviceConfiguration.Segue.selectGroup {
            let destinationVC = segue.destination as! OptionPickerViewController
            destinationVC.viewModel = viewModel?.groupPickerViewModel()
            destinationVC.emptyMessageText = "No groups"
            destinationVC.navigationItem.title = "Groups"
        } else if segue.identifier == Storyboard.DeviceConfiguration.Segue.editName {
            let destinationVC = segue.destination as! TextInputViewController
            let destinationViewModel = TextInputViewModel { [weak self] (text) in
                self?.name.text = text
                self?.viewModel?.changeNameTo(text!)
            }
            destinationViewModel.text = name.text
            destinationVC.viewModel = destinationViewModel
        } else if segue.identifier == Storyboard.DeviceConfiguration.Segue.selectFunctionality {
            let destinationVC = segue.destination as! OptionPickerViewController
            destinationVC.viewModel = viewModel?.functionalityPickerViewModel()
            destinationVC.emptyMessageText = "No functionalities"
            destinationVC.navigationItem.title = "Functionalities"
        }
    }

    @IBAction private func proxySwitchChanged(_ sender: UISwitch) {
        if !sender.isOn, let viewModel = viewModel, viewModel.device.isConnectedAsProxy() {
            proxyNotification()
        } else {
            setProxy(sender.isOn)
        }
    }
    
    private func proxyNotification() {
        let title = "Disable proxy"
        let message = "Disable of proxy will break active Bluetooth connection. Do you want to do it?"
        let alert = UIAlertController(title: title, message: message) { [weak self] ok in
            ok ? self?.setProxy(false) : self?.proxySwitch.setOn(true, animated: false)
        }
        present(alert, animated: true, completion: nil)
    }
    
    private func setProxy(_ enabled: Bool) {
        SVProgressHUD.show(withStatus: enabled ? "Turning Proxy On..." : "Turning Proxy Off...")
        viewModel?.setProxy(enabled) { success in
            if success {
                SVProgressHUD.showSuccess(withStatus: "Proxy updated")
            } else {
                SVProgressHUD.showError(withStatus: "Proxy failed to update")
            }
            SVProgressHUD.dismiss(withDelay: 1.5)
        }
    }
    
    @IBAction private func relaySwitchChanged(_ sender: UISwitch) {
        SVProgressHUD.show(withStatus: sender.isOn ? "Turning Relay On..." : "Turning Relay Off...")
        viewModel?.setRelay(sender.isOn) { success in
            if success {
                SVProgressHUD.showSuccess(withStatus: "Relay updated")
            } else {
                SVProgressHUD.showError(withStatus: "Relay failed to update")
            }
            SVProgressHUD.dismiss(withDelay: 1.5)
        }
    }
    
    @IBAction private func friendSwitchChanged(_ sender: UISwitch) {
        SVProgressHUD.show(withStatus: sender.isOn ? "Turning Friend On..." : "Turning Friend Off...")
        viewModel?.setFriend(sender.isOn) { success in
            if success {
                SVProgressHUD.showSuccess(withStatus: "Friend updated")
            } else {
                SVProgressHUD.showError(withStatus: "Friend failed to update")
            }
            SVProgressHUD.dismiss(withDelay: 1.5)
        }
    }
}
