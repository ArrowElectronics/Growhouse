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
    var viewModel : DeviceProvisioningViewModel?
    
    private let disposeBag = DisposeBag()
    
    @IBOutlet private weak var doneButton: UIBarButtonItem!
    @IBOutlet private weak var nameTextField: UITextField!
    @IBOutlet private weak var networkNameLabel: UILabel!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        nameTextField.text = viewModel!.name.value
        nameTextField.rx.text.bind(to: viewModel!.name).disposed(by: disposeBag)
        nameTextField.becomeFirstResponder()
        
        viewModel!.canProvisionDevice.bind(to: doneButton.rx.isEnabled).disposed(by: disposeBag)
        viewModel!.networkName.asObservable().bind(to: networkNameLabel.rx.text).disposed(by: disposeBag)
    }
    
    @IBAction func cancel(_ sender: UIBarButtonItem) {
        navigationController?.popViewController(animated: true)
    }
    
    @IBAction private func done(_ sender: Any) {
        nameTextField.resignFirstResponder()
        viewModel?.done()
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        switch segue.identifier {
        case Storyboard.Provision.Segue.selectNetwork?:
            let optionPicker = segue.destination as! OptionPickerViewController
            optionPicker.viewModel = viewModel!.networkPickerViewModel()
        default:
            break;
        } 
    }
}
