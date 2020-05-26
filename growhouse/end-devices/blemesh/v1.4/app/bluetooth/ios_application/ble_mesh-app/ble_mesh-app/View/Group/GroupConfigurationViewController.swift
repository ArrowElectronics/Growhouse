//
//  GroupConfigurationViewController.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 05.06.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit

class GroupConfigurationViewController: UITableViewController {

    var viewModel: GroupConfigurationViewModel!
    
    @IBOutlet private weak var groupNameLabel: UILabel!
    @IBOutlet private weak var groupIdLabel: UILabel!
    @IBOutlet private weak var networkNameLabel: UILabel!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        groupNameLabel.text = viewModel.groupName
        groupIdLabel.text = viewModel.groupId
        networkNameLabel.text = viewModel.subnetName
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        if segue.identifier == Storyboard.Group.Segue.editGroupName {
            let destinationVC = segue.destination as! TextInputViewController
            let destinationViewModel = TextInputViewModel { [weak self] (text) in
                self?.groupNameLabel.text = text
            }
            destinationViewModel.text = groupNameLabel.text
            destinationVC.viewModel = destinationViewModel
        }
    }
    
    @IBAction func cancelConfiguration(_ sender: UIBarButtonItem) {
        navigationController?.popViewController(animated: true)
    }
    
    @IBAction func saveConfiguration(_ sender: UIBarButtonItem) {
        viewModel?.changeGroupName(to: groupNameLabel.text!)
        navigationController?.popViewController(animated: true)
    }
}
