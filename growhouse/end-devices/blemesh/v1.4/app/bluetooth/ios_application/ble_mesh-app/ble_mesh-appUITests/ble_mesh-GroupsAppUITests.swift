//
//  ble_mesh-GroupsAppUITests.swift
//  ble_mesh-appUITests
//
//  Created by Aleksandra Bocheńska on 11/07/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import XCTest

class ble_mesh_GroupsAppUITests: XCTestCase {
        
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
        testHelper.navigateToNetwork()
    }
    
    override func tearDown() {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
        Springboard.shared.deleteApp(withName: "Bluetooth Mesh")
        super.tearDown()
    }
    
    func testAddGroup() {
        // Add new group to the network
        let app = XCUIApplication()
        app/*@START_MENU_TOKEN@*/.buttons["Groups"]/*[[".segmentedControls.buttons[\"Groups\"]",".buttons[\"Groups\"]"],[[[-1,1],[-1,0]]],[0]]@END_MENU_TOKEN@*/.tap()
        XCTAssertTrue(app.staticTexts["No groups"].exists)
        app.navigationBars["Network"].buttons["Add"].tap()
        app.typeText("Group")
        app.navigationBars["Add group"].buttons["Done"].tap()
    }

    
    func testNavigatingToAndFromGroupWhenNotConnected() {
        TestHelper().addGroup()
        // Try to enter the Group while no connection to the network established
        let app = XCUIApplication()
        app.tables/*@START_MENU_TOKEN@*/.staticTexts["0 devices"]/*[[".cells.staticTexts[\"0 devices\"]",".staticTexts[\"0 devices\"]"],[[[-1,1],[-1,0]]],[0]]@END_MENU_TOKEN@*/.tap()
        XCTAssertTrue(app/*@START_MENU_TOKEN@*/.otherElements["You are not connected to network"]/*[[".otherElements[\"You are not connected to network\"]",".otherElements[\"SVProgressHUD\"]"],[[[-1,1],[-1,0]]],[1]]@END_MENU_TOKEN@*/.exists)
    }
    
}
