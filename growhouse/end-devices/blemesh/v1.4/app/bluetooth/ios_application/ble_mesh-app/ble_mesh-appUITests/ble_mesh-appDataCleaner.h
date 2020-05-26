//
//  ble_mesh-appDataCleaner.h
//  ble_mesh-appUITests
//
//  Created by Aleksandra Bocheńska on 12/07/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#ifndef XCTest_Private_h
#define XCTest_Private_h

#import <XCTest/XCUIApplication.h>

@interface XCUIApplication (Private)
- (id)initPrivateWithPath:(id)arg1 bundleID:(id)arg2;
@end

@interface XCUIElement (Private)
- (void)resolve;
@end

#endif /* XCTest_Private_h */
