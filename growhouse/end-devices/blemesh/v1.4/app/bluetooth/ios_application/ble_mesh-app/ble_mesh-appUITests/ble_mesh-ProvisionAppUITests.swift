//
//  ble_mesh-ProvisionAppUITests.swift
//  ble_mesh-appUITests
//
//  Created by Aleksandra Bocheńska on 11/07/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import XCTest

class ble_mesh_ProvisionAppUITests: XCTestCase {
        
    override func setUp() {
        super.setUp()
        
        // Put setup code here. This method is called before the invocation of each test method in the class.
        
        // In UI tests it is usually best to stop immediately when a failure occurs.
        continueAfterFailure = false
        // UI tests must launch the application that they test. Doing this in setup will make sure it happens for each test method.
        XCUIApplication().launch()

        // In UI tests it’s important to set the initial state - such as interface orientation - required for your tests before they run. The setUp method is a good place to do this.
        
        let testHelper = TestHelper()
        testHelper.addNetwork()


    }
    
    override func tearDown() {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
        Springboard.shared.deleteApp(withName: "Bluetooth Mesh")
        super.tearDown()
    }
    
    func testNavigateToProvisionBluetoothDisabled() {
        // Enter provision view when Bluetooth disabled
        let app = XCUIApplication()
        app.tabBars.buttons["Provision"].tap()
        XCTAssertTrue(app/*@START_MENU_TOKEN@*/.otherElements["Bluetooth Disabled"]/*[[".otherElements[\"Bluetooth Disabled\"]",".otherElements[\"SVProgressHUD\"]"],[[[-1,1],[-1,0]]],[1]]@END_MENU_TOKEN@*/.exists)
    }
    
    func testNavigateToProvisionBluetoothEnabled() {
        // Enter provision view when Bluetooth enabled
        let app = XCUIApplication()
        app.tabBars.buttons["Provision"].tap()
        XCTAssertFalse(app/*@START_MENU_TOKEN@*/.otherElements["Bluetooth Disabled"]/*[[".otherElements[\"Bluetooth Disabled\"]",".otherElements[\"SVProgressHUD\"]"],[[[-1,1],[-1,0]]],[1]]@END_MENU_TOKEN@*/.exists)
    }
    
    func testStartAndStopScanning() {
        // Start and stop scanning for nodes
        let app = XCUIApplication()
        app.tabBars.buttons["Provision"].tap()
        XCTAssertTrue(app.tables["No devices\n\nTap Scan to start searching for Bluetooth mesh devices."].staticTexts["No devices\n\nTap Scan to start searching for Bluetooth mesh devices."].exists)
        
        let provisionNavigationBar = app.navigationBars["Provision"]

        app.tabBars.buttons["Provision"].tap()
        provisionNavigationBar.buttons["scan_button"].tap()
        XCTAssertTrue(app.children(matching: .window).element(boundBy: 0).children(matching: .other).element.children(matching: .other).element.children(matching: .other).element.children(matching: .other).element.children(matching: .other).element.children(matching: .other).element.children(matching: .table).element.exists)
        provisionNavigationBar.buttons["scan_button"].tap()
    }
    
}
