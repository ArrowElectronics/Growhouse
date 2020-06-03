//
//  AppDelegate.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 27/03/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import SwiftyBeaver
import BluetoothMesh
import SVProgressHUD

let log = SwiftyBeaver.self

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate, SBMLoggerDelegate {

    var window: UIWindow?

    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplicationLaunchOptionsKey: Any]?) -> Bool {
        setupLogs()
        setupAppearance()
        setupSVProgressHUD()
        setupApplication()
        
        return true
    }

    private func setupLogs() {
        let console = ConsoleDestination()
        
        log.addDestination(console)
        
        SBMLogger.sharedInstance().delegate = self;
    }
    
    func logMessage(_ message: String?, with level: SBMLogLevel) {
        guard !(message ?? "").contains("Mesh: libble_mesh: mesh_stack_message_deliver:") else {
            return
        }
        
        if let message = message {
            switch level {
            case SBMLogLevelVerbose:
                log.verbose(message)
            case SBMLogLevelDebug:
                log.debug(message)
            case SBMLogLevelInfo:
                log.info(message)
            case SBMLogLevelWarn:
                log.warning(message)
            case SBMLogLevelError:
                log.error(message)
            default:
                log.verbose(message)
            }
        }
    }

    private func setupApplication() {
        ViewModelServices.sharedInstance.peripheralDiscovery.startBluetoothService()
    }

    private func setupAppearance() {
        let grayColor = UIColor(red: 84.0/255.0, green: 84.0/255.0, blue: 84.0/255.0, alpha: 1.0)
        let lightGrayColor = UIColor(red: 50.0/255.0, green: 50.0/255.0, blue: 51.0/255.0, alpha: 1.0)
        let redColor = UIColor(red: 217.0/255.0, green: 31.0/255.0, blue: 42.0/255.0, alpha: 1.0)
        
        UINavigationBar.appearance().barTintColor = redColor
        UINavigationBar.appearance().tintColor = .white;
        UINavigationBar.appearance().titleTextAttributes = [.foregroundColor: UIColor.white,
                                                            .font: UIFont.systemFont(ofSize: 17, weight: .semibold)]
        
        UITabBar.appearance().tintColor = redColor
        UITableView.appearance().separatorColor = lightGrayColor
        UITableViewCell.appearance().backgroundColor = grayColor
    }
    
    private func setupSVProgressHUD() {
        SVProgressHUD.setDefaultMaskType(.black)
    }
}

