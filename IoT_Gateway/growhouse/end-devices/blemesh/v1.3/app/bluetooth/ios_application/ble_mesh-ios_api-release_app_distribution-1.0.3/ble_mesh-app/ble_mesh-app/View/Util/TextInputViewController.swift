//
//  TextInputViewController.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 11/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import SVProgressHUD

class TextInputViewController: UITableViewController, UITextFieldDelegate {
    
    var viewModel : TextInputViewModel?
    
    @IBOutlet private weak var textField: UITextField!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        textField.text = viewModel?.text
        textField.delegate = self
        textField.attributedPlaceholder = NSAttributedString(string: viewModel?.placeholder ?? "", attributes: [NSAttributedStringKey.foregroundColor : UIColor.lightGray])
        textField.becomeFirstResponder()
    }
    
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        guard !textField.text!.trimmingCharacters(in: .whitespacesAndNewlines).isEmpty else {
            SVProgressHUD.showError(withStatus: "Invalid text input")
            SVProgressHUD.dismiss(withDelay: 1.0)
            return false
        }
        
        textField.resignFirstResponder()
        viewModel?.didReturn(textField.text)
        navigationController?.popViewController(animated: true)
        return true
    }
    
    @IBAction func cancel(_ sender: UIBarButtonItem) {
        navigationController?.popViewController(animated: true)
    }
}
