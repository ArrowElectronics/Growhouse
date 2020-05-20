//
//  AddGroupViewController.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 23/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import RxSwift
import RxCocoa

class AddGroupViewController: UIViewController {
    
    var viewModel: AddGroupViewModel!
    
    @IBOutlet private weak var nameTextField: UITextField!
    @IBOutlet private weak var addGroupButton: UIBarButtonItem!
    
    private let disposeBag = DisposeBag()

    override func viewDidLoad() {
        super.viewDidLoad()

        nameTextField.becomeFirstResponder()
        nameTextField.rx.text.bind(to: viewModel.name).disposed(by: disposeBag)
        
        viewModel.canAddGroup.bind(to: addGroupButton.rx.isEnabled).disposed(by: disposeBag)
    }
    
    @IBAction private func addGroup(_ sender: Any) {
        viewModel.addGroup()
        self.navigationController?.popViewController(animated: true)
    }
}
