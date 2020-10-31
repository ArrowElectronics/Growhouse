//
//  ble_mesh-NetworkAppUITests.swift
//  ble_mesh-appUITests
//
//  Created by Aleksandra Bocheńska on 11/07/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import XCTest

class ble_mesh_NetworkAppUITests: XCTestCase {
        
    override func setUp() {
        super.setUp()
        
        // Put setup code here. This method is called before the invocation of each test method in the class.
        
        // In UI tests it is usually best to stop immediately when a failure occurs.
        continueAfterFailure = false
        // UI tests must launch the application that they test. Doing this in setup will make sure it happens for each test method.
        XCUIApplication().launch()

        // In UI tests it’s important to set the initial state - such as interface orientation - required for your tests before they run. The setUp method is a good place to do this.
    }
    
    override func tearDown() {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
        super.tearDown()
    }
    
    func testAddNetwork() {
        // Add new network
        let app = XCUIApplication()
        app.navigationBars["Networks"].buttons["Add"].tap()
        app.typeText("Network")
        app.navigationBars["Add network"].buttons["Done"].tap()
    }
    
    func testNavigatingToAndFromNetwork() {
        // Enter network view and go back to networks list
        let app = XCUIApplication()
        app.tables.cells.children(matching: .other).matching(identifier: "network field").element(boundBy: 1).tap()
        app.navigationBars["Network"].buttons["Networks"].tap()
    }
    
}
