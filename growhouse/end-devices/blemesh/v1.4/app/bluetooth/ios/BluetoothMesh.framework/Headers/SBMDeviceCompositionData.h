//
//  SBMDeviceCompositionData.h
//  ble_mesh-lib
//
//  Created by Lukasz Rzepka on 04.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMElement.h"

/**
 SBMDeviceCompositionData contains information about a node, the elements it includes, and the supported models.
 */
@interface SBMDeviceCompositionData : NSObject

/**
 Company identifier.
 */
@property (nonatomic, readonly) NSInteger cid;

/**
 Vendor-assigned product identifier.
 */
@property (nonatomic, readonly) NSInteger pid;

/**
 Vendor-assigned product version identifier.
 */
@property (nonatomic, readonly) NSInteger vid;

/**
 Minimum number of replay protection list entries in a device.
 */
@property (nonatomic, readonly) NSInteger crpl;

/**
 Device features:
 ```
 Bit 0: Relay. Relay feature support: 0 = False, 1 = True.
 Bit 1: Proxy. Proxy feature support: 0 = False, 1 = True.
 Bit 2: Friend. Friend feature support: 0 = False, 1 = True.
 Bit 3: Low Power. Low Power feature support: 0 = False, 1 = True.
 Bit 4–15: RFU. Reserved for Future Use
 ```
 */
@property (nonatomic, readonly) NSInteger feature;

///-------------------------------
/// @name Feature Support
///-------------------------------

/**
 Is Relay feature supported by node.
 */
- (BOOL)supportsRelay;

/**
 Is Proxy feature supported by node.
 */
- (BOOL)supportsProxy;

/**
 Is Friend feature supported by node.
 */
- (BOOL)supportsFriend;

/**
 Is Low Power node.
 */
- (BOOL)isLowPower;

/**
 Not available.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available.
 */
- (instancetype)init NS_UNAVAILABLE;

@end
