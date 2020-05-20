//
//  AddNetworkViewController.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 11/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import RxSwift
import RxCocoa

class AddNetworkViewController: UIViewController {
    
    var viewModel = AddNetworkViewModel()
    
    @IBOutlet private weak var networkName: UITextField!
    @IBOutlet private weak var addNetworkButton: UIBarButtonItem!
    
    private let disposeBag = DisposeBag()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        networkName.becomeFirstResponder()
        networkName.rx.text.bind(to: viewModel.name).disposed(by: disposeBag)
        
        viewModel.canAddNetwork.bind(to: addNetworkButton.rx.isEnabled).disposed(by: disposeBag)
    }
    
    @IBAction func cancelNetwork(_ sender: UIBarButtonItem) {
        navigationController?.popViewController(animated: true)
    }
    
    @IBAction private func addNetwork(_ sender: UIBarButtonItem) {
        viewModel.addNetwork()
        
        networkName.resignFirstResponder()
        navigationController?.popViewController(animated: true)
    }
}
