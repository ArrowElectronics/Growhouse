//
//  AppDelegate.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 27/03/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit
import CocoaLumberjack
import BluetoothMesh
import SVProgressHUD

@UIApplicationMain class AppDelegate: UIResponder, UIApplicationDelegate, SBMLoggerDelegate {
    
    var window: UIWindow?
    
    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplicationLaunchOptionsKey: Any]?) -> Bool {
        setupLogs()
        setupAppearance()
        setupSVProgressHUD()
        
        return true
    }
    
    func logMessage(_ message: String?, with level: SBMLogLevel) {
        guard !(message ?? "").contains("Mesh: libble_mesh: mesh_stack_message_deliver:") else {
            return
        }
        
        if let message = message {
            switch level {
            case SBMLogLevelVerbose:
                DDLogVerbose(message)
            case SBMLogLevelDebug:
                DDLogDebug(message)
            case SBMLogLevelInfo:
                DDLogInfo(message)
            case SBMLogLevelWarn:
                DDLogWarn(message)
            case SBMLogLevelError:
                DDLogError(message)
            default:
                DDLogVerbose(message)
            }
        }
    }
    
    private func setupLogs() {
        DDLog.add(DDOSLogger.sharedInstance)
        
        let fileLogger: DDFileLogger = DDFileLogger() // File Logger
        fileLogger.rollingFrequency = 60 * 60 * 24 // 24 hours
        fileLogger.logFileManager.maximumNumberOfLogFiles = 7
        DDLog.add(fileLogger)
        
        SBMLogger.sharedInstance().delegate = self;
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
