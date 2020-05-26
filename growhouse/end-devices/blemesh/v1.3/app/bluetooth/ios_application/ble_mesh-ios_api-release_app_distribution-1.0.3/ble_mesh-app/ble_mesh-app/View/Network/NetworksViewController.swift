//
//  NetworksViewController.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 12/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import RxSwift
import RxCocoa
import SWTableViewCell

class NetworksViewController: UIViewController, UITableViewDelegate, UITableViewDataSource, SWTableViewCellDelegate, SiliconLabsBrandedViewController {
    
    var viewModel = NetworksViewModel()
    
    @IBOutlet private weak var tableView: UITableView!
    private var emptyMessageLabel: UILabel!
    
    private var networks = [NetworkCellViewModel]()
    private let disposeBag = DisposeBag()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        setupEmptyMessageLabel()

        tableView.delegate = self
        tableView.dataSource = self
        viewModel.networks.asDriver().drive(onNext: { [weak self] (networks) in
            self?.networks = networks
            self?.tableView.reloadData()
        }).disposed(by: disposeBag)
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        
        setLogoAsTitleOnNavigationItem()
    }
    
    private func setupEmptyMessageLabel() {
        let mutableAttString = NSMutableAttributedString()
        let noNetworksString = NSAttributedString(string: "No networks\n\n", attributes: [ .font : UIFont.systemFont(ofSize: 17, weight: .semibold) ])
        let tapToCreateString = NSAttributedString(string: "Tap + to create Bluetooth\nmesh network.", attributes: [ .font : UIFont.systemFont(ofSize: 15) ])
        
        mutableAttString.append(noNetworksString)
        mutableAttString.append(tapToCreateString)
        
        emptyMessageLabel = UILabel(frame: view.bounds)
        emptyMessageLabel.attributedText = mutableAttString
        emptyMessageLabel.textColor = .white
        emptyMessageLabel.numberOfLines = 0;
        emptyMessageLabel.textAlignment = .center;
        emptyMessageLabel.sizeToFit()
        tableView.backgroundView = emptyMessageLabel;
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: Storyboard.Network.CellIdentifiers.networkCell) as! NetworkCellView
        let network = networks[indexPath.row]
        
        cell.name = network.name
        cell.info = network.info
        cell.delegate = self
        cell.rightUtilityButtons = [UIButton.deleteButton, UIButton.configureButton]
        
        return cell
    }
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        emptyMessageLabel.isHidden = networks.count > 0
        
        return networks.count
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        let cellViewModel = networks[indexPath.row]
        let networkDetailViewModel = NetworkDetailViewModel(viewModel.services, meshNetwork: cellViewModel.network)
        tableView.deselectRow(at: indexPath, animated: true)
        
        performSegue(withIdentifier: Storyboard.Network.Segue.network, sender: networkDetailViewModel)
    }
    
    func swipeableTableViewCell(_ cell: SWTableViewCell!, didTriggerRightUtilityButtonWith index: Int) {
        if index == 0 {
            viewModel.remove(network: networks[(tableView.indexPath(for: cell)?.row)!].network)
        } else if index == 1 {
            let cellViewModel = networks[(tableView.indexPath(for: cell)?.row)!]
            let networkConfigurationViewModel = NetworkConfigurationViewModel(viewModel.services, network: cellViewModel.network)
            performSegue(withIdentifier: Storyboard.Network.Segue.networkConfiguration, sender: networkConfigurationViewModel)
        }
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        switch segue.identifier {
        case Storyboard.Network.Segue.network:
            let viewController = segue.destination as! NetworkDetailViewController
            viewController.viewModel = sender as! NetworkDetailViewModel
            viewController.hidesBottomBarWhenPushed = true
        case Storyboard.Network.Segue.networkConfiguration:
            let viewController = segue.destination as! NetworkConfigurationViewController
            viewController.viewModel = sender as! NetworkConfigurationViewModel
            viewController.hidesBottomBarWhenPushed = true
        default:
            break;
        }
    }
}
