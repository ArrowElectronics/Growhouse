//
//  GroupsViewController.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 23/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import SVProgressHUD
import RxSwift
import SWTableViewCell

class GroupsViewController: UIViewController, UITableViewDelegate, UITableViewDataSource, SWTableViewCellDelegate {
    
    var viewModel : GroupsViewModel!
    
    @IBOutlet private weak var emptyMessage: UIView!
    @IBOutlet private weak var tableView: UITableView!
    @IBOutlet private var separatorViews: [UIView]!
    private var addButton: UIBarButtonItem?
    
    private var groups = [GroupCellViewModel]()
    private let disposeBag = DisposeBag()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        tableView.delegate = self;
        tableView.dataSource = self;
        
        viewModel.groups.asDriver().drive(onNext: { [weak self] (groups) in
            self?.groups = groups
            
            for separatorView in self?.separatorViews ?? [] {
                separatorView.isHidden = groups.count == 0
            }
            
            self?.emptyMessage.isHidden = groups.count != 0
            self?.tableView.reloadData()
        }).disposed(by: disposeBag)
        
        viewModel.canAddGroup.asDriver().drive(onNext: { [weak self] (canAddGroup) in
            self?.addButton?.isEnabled = canAddGroup
        }).disposed(by: disposeBag)
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        
        let addButton = UIBarButtonItem(barButtonSystemItem: .add, target: self, action: #selector(addButtonTapped(_:)))
        addButton.isEnabled = viewModel.canAddGroup.value
        self.parent?.parent?.navigationItem.rightBarButtonItems?.insert(addButton, at: 0)
        self.addButton = addButton
    
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
    }
    
    override func viewDidDisappear(_ animated: Bool) {
        self.parent?.parent?.navigationItem.rightBarButtonItems?.removeFirst()
        super.viewDidDisappear(animated)
    }
    
    @objc
    private func addButtonTapped(_ sender: UIBarButtonItem) {
        self.performSegue(withIdentifier: Storyboard.Group.Segue.addGroup, sender: sender)
    }
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return groups.count;
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: Storyboard.Group.Cell.groupCell) as! GroupCellView
        cell.viewModel = groups[indexPath.row]
        cell.delegate = self
        cell.rightUtilityButtons = [UIButton.deleteButton, UIButton.configureButton]
        cell.separatorLine.isHidden = indexPath.row == groups.count - 1
        
        return cell;
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        if viewModel.isConnected.value {
            performSegue(withIdentifier: Storyboard.Group.Segue.showGroup, sender: groups[indexPath.row].name)
        } else {
            SVProgressHUD.showError(withStatus: "You are not connected to network")
            SVProgressHUD.dismiss(withDelay: 1.0)
        }
        
        tableView.deselectRow(at: indexPath, animated: true)
    }
    
    func swipeableTableViewCell(_ cell: SWTableViewCell!, didTriggerRightUtilityButtonWith index: Int) {
        let cellViewModel = groups[(tableView.indexPath(for: cell)?.row)!]
        
        if index == 0 {
            self.confirmDelete(group: cellViewModel, selectedCell: cell)
        } else if index == 1 {
            let groupConfigurationViewModel = GroupConfigurationViewModel(viewModel.services, group: cellViewModel.group)
            cell.hideUtilityButtons(animated: false)
            performSegue(withIdentifier: Storyboard.Group.Segue.groupConfiguration, sender: groupConfigurationViewModel)
        }
    }
    
    private func confirmDelete(group: GroupCellViewModel, selectedCell: SWTableViewCell) {
        let title = "Delete group"
        let message = "Are you sure you want to delete the selected group?"
        let alert = UIAlertController(title: title, message: message) { [weak self] ok in
            ok ? self?.remove(group: group) : selectedCell.hideUtilityButtons(animated: true)
        }
        present(alert, animated: true, completion: nil)
    }
    
    private func remove(group: GroupCellViewModel) {
        SVProgressHUD.show(withStatus: "Removing group")
        viewModel.remove(group: group) { [weak self] in
            self?.showPopup(for: $0, group: group)
        }
    }
    
    private func showPopup(for nodes: [String]?, group: GroupCellViewModel) {
        if let nodes = nodes {
            let title = "Could not unbind nodes: \(nodes)"
            let message = "Do you want to force-remove the selected group anyway?"
            let alert = UIAlertController(title: title, message: message, cancelTitle: "No", destructiveTitle: "Yes") { [weak self] ok in
                ok ? self?.viewModel.forceRemove(group: group) : nil
            }
            present(alert, animated: true, completion: nil)
            SVProgressHUD.dismiss()
        } else {
            SVProgressHUD.showSuccess(withStatus: "Group removed")
            SVProgressHUD.dismiss(withDelay: 1.5)
        }
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        switch segue.identifier {
        case Storyboard.Group.Segue.addGroup:
            let addGroup = segue.destination as! AddGroupViewController
            addGroup.viewModel = viewModel.addGroupViewModel
        case Storyboard.Group.Segue.showGroup:
            let destinationViewController = segue.destination as! GroupViewController
            let group = viewModel.subnet.groups.first { $0.name == sender as? String }
            let vm = GroupViewModel(viewModel.services, group: group!, subnet: viewModel.subnet)
            let devicesVM = ProvisionedDevicesViewModel(viewModel.services, subnet: viewModel.subnet, group: group!)
            destinationViewController.viewModel = vm
            destinationViewController.devicesViewModel = devicesVM
            destinationViewController.title = sender as? String
        case Storyboard.Group.Segue.groupConfiguration:
            let viewController = segue.destination as! GroupConfigurationViewController
            viewController.viewModel = sender as? GroupConfigurationViewModel
        default:
            break
        }
    }
}
