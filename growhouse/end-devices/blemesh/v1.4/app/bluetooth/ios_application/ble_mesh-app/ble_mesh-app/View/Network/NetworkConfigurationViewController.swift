//
//  NetworkConfigurationViewController.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 05.06.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import SVProgressHUD
import RxSwift
import RxCocoa

class NetworkConfigurationViewController: UIViewController {
    
    var viewModel: NetworkConfigurationViewModel!

    @IBOutlet private weak var saveButton: UIBarButtonItem!
    @IBOutlet private weak var networkNameLabel: UILabel!
    @IBOutlet private weak var networkNidLabel: UILabel!
    @IBOutlet private weak var networkKeyLabel: UILabel!
    
    private let disposeBag = DisposeBag()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        networkNameLabel.text = viewModel.networkName.value
        networkKeyLabel.text = viewModel.subnet.netKey.rawKey.hexEncodedString()
        networkNidLabel.text = "\(viewModel.subnet.netKey.keyIndex)"
        
        viewModel.canSaveName.bind(to: saveButton.rx.isEnabled).disposed(by: disposeBag)
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        if segue.identifier == Storyboard.Network.Segue.editNetworkName {
            let destinationVC = segue.destination as! TextInputViewController
            let destinationViewModel = TextInputViewModel { [weak self] (text) in
                self?.networkNameLabel.text = text
            }
            destinationViewModel.text = networkNameLabel.text
            destinationVC.viewModel = destinationViewModel
        }
    }
    
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        guard !textField.text!.trimmingCharacters(in: .whitespacesAndNewlines).isEmpty else {
            SVProgressHUD.showError(withStatus: "Invalid text input")
            SVProgressHUD.dismiss(withDelay: 1.0)
            return false
        }
        
        textField.resignFirstResponder()
        return true
    }
    
    @IBAction func cancelNetworkConfiguration(_ sender: UIBarButtonItem) {
        navigationController?.popViewController(animated: true)
    }
    
    @IBAction func saveButtonTapped(_ sender: UIBarButtonItem) {
        viewModel.changeNetworkName(to: networkNameLabel.text!)
        navigationController?.popViewController(animated: true)
    }
}
