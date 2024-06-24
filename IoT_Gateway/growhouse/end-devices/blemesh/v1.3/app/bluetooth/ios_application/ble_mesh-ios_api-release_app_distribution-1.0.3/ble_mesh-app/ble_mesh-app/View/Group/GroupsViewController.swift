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
    
    @IBOutlet private weak var tableView: UITableView!
    @IBOutlet private weak var emptyMessage: UILabel!
    @IBOutlet private var separatorViews: [UIView]!
    
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
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        
        let addButton = UIBarButtonItem(barButtonSystemItem: .add, target: self, action: #selector(addButtonTapped(_:)))
        self.parent?.parent?.navigationItem.rightBarButtonItems?.insert(addButton, at: 0)
    
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
        self.performSegue(withIdentifier: Storyboard.Network.Segue.addGroup, sender: sender)
    }
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return groups.count;
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: Storyboard.Network.CellIdentifiers.groupCell) as! GroupCellView
        cell.viewModel = groups[indexPath.row]
        cell.delegate = self
        cell.rightUtilityButtons = [UIButton.deleteButton, UIButton.configureButton]
        cell.separatorLine.isHidden = indexPath.row == groups.count - 1
        
        return cell;
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        if viewModel.isConnected {
            performSegue(withIdentifier: Storyboard.Network.Segue.showGroup, sender: groups[indexPath.row].name)
        } else {
            SVProgressHUD.showError(withStatus: "You are not connected to network")
            SVProgressHUD.dismiss(withDelay: 1.0)
        }
        
        tableView.deselectRow(at: indexPath, animated: true)
    }
    
    func swipeableTableViewCell(_ cell: SWTableViewCell!, didTriggerRightUtilityButtonWith index: Int) {
        let cellViewModel = groups[(tableView.indexPath(for: cell)?.row)!]
        
        if index == 0 {
            self.viewModel.remove(group: cellViewModel)
        } else if index == 1 {
            let groupConfigurationViewModel = GroupConfigurationViewModel(viewModel.services, group: cellViewModel.group)
            performSegue(withIdentifier: Storyboard.Network.Segue.groupConfiguration, sender: groupConfigurationViewModel)
        }
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        switch segue.identifier {
        case Storyboard.Network.Segue.addGroup:
            let addGroup = segue.destination as! AddGroupViewController
            addGroup.viewModel = viewModel.addGroupViewModel()
        case Storyboard.Network.Segue.showGroup:
            let destinationViewController = segue.destination as! GroupViewController
            let group = viewModel.services.networkManager.getGroups(withNetwork: viewModel.network).first { $0.name == sender as? String }
            let vm = GroupViewModel(viewModel.services, group: group!, meshNetwork: viewModel.network)
            let devicesVM = ProvisionedDevicesViewModel(viewModel.services, meshNetwork: viewModel.network, meshGroup: group!)
            vm.isConnected = viewModel.isConnected
            destinationViewController.viewModel = vm
            destinationViewController.devicesViewModel = devicesVM
            destinationViewController.title = sender as? String
        case Storyboard.Network.Segue.groupConfiguration:
            let viewController = segue.destination as! GroupConfigurationViewController
            viewController.viewModel = sender as! GroupConfigurationViewModel
        default:
            break
        }
    }
}
