//
//  ProvisionViewController.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 12/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import RxSwift
import RxCocoa
import BluetoothMesh
import SVProgressHUD

class ProvisionViewController: UITableViewController, SiliconLabsBrandedViewController {
    
    var viewModel = ProvisionViewModel()
    
    @IBOutlet private weak var scanButton: UIBarButtonItem!
    @IBOutlet private var separatorViews: [UIView]!
    
    private var devices = [ProvisionCellViewModel]()
    private var devicesSubscription: Disposable?
    private var emptyMessageLabel: UILabel!
    
    private let disposeBag = DisposeBag()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        viewModel.didProvisionNodeSubject.subscribe(onNext: { [weak self] tuple in
            let (node, connection) = tuple
            
            if let indexToRemove = self?.devices.index(where: { $0.identifier == connection.peripheral.identifier }) {
                self?.devices.remove(at: indexToRemove)
                self?.tableView.reloadData()
            }
            
            CATransaction.begin()
            CATransaction.setCompletionBlock({
                self?.showConfigurationView(node: node, connection: connection)
            })
            
            self?.navigationController?.popViewController(animated: true)
            
            CATransaction.commit()
        }).disposed(by: disposeBag)
        
        viewModel.bluetoothConnected.asDriver().drive(onNext: { [weak self] (bluetoothConnected) in
            if !bluetoothConnected {
                self?.stopScanning()
                self?.refreshControl?.endRefreshing()
                self?.devices.removeAll()
                self?.tableView.reloadData()
                self?.showBluetoothDialog()
            }
        }).disposed(by: disposeBag)
        
        setScanButtonScanText()
        setupRefreshControl()
        setupEmptyMessageLabel()
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        
        setLogoAsTitleOnNavigationItem()
        setupSeparatorViewsHeight()
        updateSeparatorViews()
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        stopScanning()
    }
    
    @IBAction private func scanButtonTapped(_ sender: Any) {
        if refreshControl!.isRefreshing {
            stopScanning()
        } else {
            UIView.animate(withDuration: 0.1, delay: 0, options: .beginFromCurrentState, animations: {
                self.tableView.setContentOffset(CGPoint(x: 0, y: -self.refreshControl!.frame.size.height), animated: false)
            }) { _ in
                self.refreshControl!.beginRefreshing()
                self.startScanning()
            }
        }
    }
    
    private func showConfigurationView(node: SBMNode, connection: BluetoothDevice) {
        let networkDetailViewModel = DeviceConfigurationViewModel(viewModel.services, device: node)
        networkDetailViewModel.deviceConfigurationDetailViewModel.onDeinit = { [weak self] in
            self?.viewModel.services.bluetoothMeshService.disconnect(device: connection)
        }
        
        performSegue(withIdentifier: Storyboard.Provision.Segue.deviceConfiguration, sender: networkDetailViewModel)
    }
    
    private func setupEmptyMessageLabel() {
        let mutableAttString = NSMutableAttributedString()
        let noDevicesString = NSAttributedString(string: "No devices\n\n", attributes: [ .font : UIFont.systemFont(ofSize: 17) ])
        let tapToScanString = NSAttributedString(string: "Tap Scan to start searching for\nBluetooth mesh devices.", attributes: [ .font : UIFont.systemFont(ofSize: 15) ])
        
        mutableAttString.append(noDevicesString)
        mutableAttString.append(tapToScanString)
        
        emptyMessageLabel = UILabel(frame: view.bounds)
        emptyMessageLabel.attributedText = mutableAttString
        emptyMessageLabel?.textColor = .white
        emptyMessageLabel?.numberOfLines = 0;
        emptyMessageLabel?.textAlignment = .center;
        emptyMessageLabel?.sizeToFit()
        tableView.backgroundView = emptyMessageLabel;
    }
    
    private func setScanButtonScanText() {
        scanButton.title = "Scan"
    }
    
    private func setScanButtonStopText() {
        scanButton.title = "Stop"
    }
    
    private func setupRefreshControl() {
        let fontColor = UIColor(red: 142.0/255.0, green: 142.0/255.0, blue: 147.0/255.0, alpha: 1.0)
        refreshControl?.attributedTitle = NSAttributedString(string: "Scanning...", attributes: [NSAttributedStringKey.foregroundColor : fontColor])
        refreshControl?.addTarget(self, action: #selector(startScanning), for: .valueChanged)
    }
    
    @objc private func startScanning() {
        guard viewModel.bluetoothConnected.value else {
            refreshControl?.endRefreshing()
            showBluetoothDialog()
            return
        }
        setScanButtonStopText()
        viewModel.scanForDevices()
        devicesSubscription = viewModel.devices.observeOn(MainScheduler.instance).take(5, scheduler: MainScheduler.instance).subscribe(onNext: { [weak self] (devices) in
            self?.devices = devices
            self?.emptyMessageLabel?.isHidden = devices.count > 0
            self?.updateSeparatorViews()
            self?.tableView.reloadData()
            self?.setScanButtonStopText()
            }, onDisposed: { [weak self] in
                self?.setScanButtonScanText()
                self?.refreshControl?.endRefreshing()
                self?.viewModel.stopScanForDevices()
        })
        devicesSubscription?.disposed(by: disposeBag)
    }
    
    private func showBluetoothDialog() {
        SVProgressHUD.showError(withStatus: "Bluetooth Disabled")
        SVProgressHUD.dismiss(withDelay: 1.5)
    }
    
    private func stopScanning() {
        setScanButtonScanText()
        devicesSubscription?.dispose()
    }
    
    private func setupSeparatorViewsHeight() {
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
    
    private func updateSeparatorViews() {
        for separatorView in self.separatorViews {
            separatorView.isHidden = devices.count == 0
        }
    }

    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return devices.count
    }
    
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: Storyboard.Provision.Cell.provisionCell) as! ProvisionCellView
        
        cell.viewModel = devices[indexPath.row]
        cell.provisionButtonTag = indexPath.row
        
        return cell
    }
    
    override func shouldPerformSegue(withIdentifier identifier: String, sender: Any?) -> Bool {
        switch identifier {
        case Storyboard.Provision.Segue.deviceProvisioning:
            if !viewModel.canProvisionDevice {
                SVProgressHUD.showError(withStatus: "You have to have at least one network to provision a device")
                SVProgressHUD.dismiss(withDelay: 1.5)
            }
            return viewModel.canProvisionDevice
        default:
            return true
        }
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        segue.destination.hidesBottomBarWhenPushed = true
        switch segue.identifier {
        case Storyboard.Provision.Segue.deviceConfiguration:
            let viewController = segue.destination as? DeviceConfigurationViewController
            viewController?.viewModel = sender as? DeviceConfigurationViewModel;
        case Storyboard.Provision.Segue.deviceProvisioning:
            let viewController = segue.destination as! DeviceProvisioningViewController
            let cellIndex = (sender as! UIButton).tag
            let device = devices[cellIndex]
            
            if let viewModel = viewModel.deviceProvisioningViewModel(for: device.device) {
                viewController.viewModel = viewModel
                devicesSubscription?.dispose()
            }
        default:
            break;
        }
    }
}
