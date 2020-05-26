//
//  DeviceProvisioningViewController.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 13/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import RxSwift
import RxCocoa
import SVProgressHUD

class DeviceProvisioningViewController: UITableViewController {
    
    var viewModel: DeviceProvisioningViewModel?
    
    private let disposeBag = DisposeBag()
    
    @IBOutlet private weak var nameTextField: UITextField!
    @IBOutlet private weak var networkNameLabel: UILabel!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        setupUI()
    }
    
    private func setupUI() {
        nameTextField.text = viewModel?.name
        viewModel?.networkName.asObservable().bind(to: networkNameLabel.rx.text).disposed(by: disposeBag)
    }
    
    @IBAction private func cancel(_ sender: UIBarButtonItem) {
        navigationController?.popViewController(animated: true)
    }
    
    @IBAction private func done(_ sender: Any) {
        guard let viewModel = viewModel else { return }
        if viewModel.canProvision() {
            SVProgressHUD.show(withStatus: "Provisioning...")
            provision()
        } else {
            showPopup()
        }
    }
        
    private func provision() {
        viewModel?.provision() { node, device, error in
            if let error = error {
                SVProgressHUD.showError(withStatus: error.localizedDescription)
                SVProgressHUD.dismiss(withDelay: 1.5)
            } else {
                SVProgressHUD.showSuccess(withStatus: "Provision success")
                SVProgressHUD.dismiss(withDelay: 1.5) { [weak self] in
                    self?.viewModel?.delegate?.didProvisionDevice(node: node!, device: device)
                }
            }
        }
    }
    
    private func showPopup() {
        let title = "Device already exists in database!"
        let message = "Do you want to remove this device from local database?"
        let alert = UIAlertController(title: title, message: message, cancelTitle: "No", destructiveTitle: "Yes") { [weak self] ok in
            ok ? self?.viewModel?.removeDevice() : nil
        }
        present(alert, animated: true, completion: nil)
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        if segue.identifier == Storyboard.Provision.Segue.selectNetwork {
            let optionPicker = segue.destination as? OptionPickerViewController
            optionPicker?.viewModel = viewModel?.networkPickerViewModel
        }
    }
}
