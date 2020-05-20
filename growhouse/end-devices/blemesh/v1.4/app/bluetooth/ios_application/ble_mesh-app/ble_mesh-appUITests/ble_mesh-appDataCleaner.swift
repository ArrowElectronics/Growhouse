//
//  ble_mesh-appDataCleaner.swift
//  ble_mesh-appUITests
//
//  Created by Aleksandra Bocheńska on 12/07/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import Foundation
import XCTest

class Springboard {
    
    static let shared = Springboard()
    
    let springboard = XCUIApplication(bundleIdentifier: "com.apple.springboard")
    let settings = XCUIApplication(bundleIdentifier: "com.apple.Preferences")
    
    func deleteApp(withName name: String) {
        
        XCUIApplication().terminate()
        
        let springboard = self.springboard
        // springboard.resolve()
        
        let icon = springboard.icons[name]
        if icon.exists {
            
            let iconFrame = icon.frame
            let springboardFrame = springboard.frame
            
            //Tap & Hold the app icon in order to go to edit mode
            Thread.sleep(forTimeInterval: 0.5)
            icon.press(forDuration: 1.3)
            
            //Tap the little "X" button at approximately where it is. The X is not exposed directly
            Thread.sleep(forTimeInterval: 0.5)
            springboard.coordinate(withNormalizedOffset: CGVector(dx: (iconFrame.minX + 3) / springboardFrame.maxX, dy: (iconFrame.minY + 3) / springboardFrame.maxY)).tap()
            
            //tap the delete alert button
            Thread.sleep(forTimeInterval: 0.5)
            springboard.alerts.buttons["Delete"].tap()
        }
        
        //Press home once make the icons stop wiggling
        Thread.sleep(forTimeInterval: 0.5)
        XCUIDevice.shared.press(.home)
    }
}
