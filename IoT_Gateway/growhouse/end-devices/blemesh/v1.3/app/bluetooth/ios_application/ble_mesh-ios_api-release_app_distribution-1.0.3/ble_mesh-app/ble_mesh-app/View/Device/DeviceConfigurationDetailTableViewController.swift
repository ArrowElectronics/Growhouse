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
        
        viewModel?.checkNodeConfiguration()
    }
    
    override func shouldPerformSegue(withIdentifier identifier: String, sender: Any?) -> Bool {
        if identifier == Storyboard.DeviceConfiguration.Segue.selectFunctionality {
            if viewModel?.device.configuration?.configuredModels(forElementIndex: 0) != nil {
                return true
            }
            
            SVProgressHUD.showError(withStatus: "You need to add device to group")
            SVProgressHUD.dismiss(withDelay: 1.5)
            return false
        }
        
        return true
    }
    
    override func tableView(_ tableView: UITableView, heightForRowAt indexPath: IndexPath) -> CGFloat {
        if indexPath.section == 0 {
            return calculateHeightForFeatureCells(indexPath)
        } else {
            return tableView.rowHeight
        }
    }
    
    func calculateHeightForFeatureCells(_ indexPath: IndexPath) -> CGFloat {
        if indexPath.row == friendCell.tag {
            return calculateHeightForSupportFriendCell()
        } else if indexPath.row == lowPowerCell.tag {
            return calculateHeightForLowPowerCell()
        } else {
            return tableView.rowHeight
        }
    }
    
    func calculateHeightForSupportFriendCell() -> CGFloat {
        if viewModel?.supportFriend.value == true {
            return tableView.rowHeight
        } else {
            return 0
        }
    }
    
    func calculateHeightForLowPowerCell() -> CGFloat {
        if viewModel?.supportLowPower.value == true {
            return tableView.rowHeight
        } else {
            return 0
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
        viewModel?.changeProxyTo(sender.isOn)
    }
    
    @IBAction private func relaySwitchChanged(_ sender: UISwitch) {
        viewModel?.changeRelayTo(sender.isOn)
    }
    
    @IBAction func friendSwitchChanged(_ sender: UISwitch) {
        viewModel?.changeFriendTo(sender.isOn)
    }
}
