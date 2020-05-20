//
//  ble_mesh_appTestHelper.swift
//  ble_mesh-appUITests
//
//  Created by Aleksandra Bocheńska on 12/07/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import Foundation
import XCTest

class TestHelper {

    // MARK: - Network
    func addNetwork() {
        let app = XCUIApplication()
        app.navigationBars["Networks"].buttons["Add"].tap()
        app.typeText("Network")
        app.navigationBars["Add network"].buttons["Done"].tap()
    }
    
    func navigateToNetwork() {
        let app = XCUIApplication()
        app.tables.otherElements["network field"].tap()
    }
    
    // MARK: - Group
    func addGroup() {
        let app = XCUIApplication()
        app/*@START_MENU_TOKEN@*/.buttons["Groups"]/*[[".segmentedControls.buttons[\"Groups\"]",".buttons[\"Groups\"]"],[[[-1,1],[-1,0]]],[0]]@END_MENU_TOKEN@*/.tap()
        app.navigationBars["Network"].buttons["Add"].tap()
        app.typeText("Group")
        app.navigationBars["Add group"].buttons["Done"].tap()
    }
    
    
}
