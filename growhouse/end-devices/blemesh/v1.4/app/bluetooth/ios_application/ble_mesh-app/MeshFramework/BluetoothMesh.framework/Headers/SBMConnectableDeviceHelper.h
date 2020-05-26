//
//  SBMConnectableDeviceHelper.h
//  BluetoothMesh
//
//  Created by Łukasz Rzepka on 30.11.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMConnectableDevice.h"
#import "SBMSubnet.h"
#import "SBMNode.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Helper class that contains methods to retrieve objects associated with given device.
 */
@interface SBMConnectableDeviceHelper : NSObject

/**
 Not available. Use class methods instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use class methods instead.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 Method used to filter devices that are unprovisioned.

 @param devices Array of devices to filter.
 @return Array of devices that are unprovisioned.
 */
+ (NSArray<id<SBMConnectableDevice>> *)filterUnprovisionedDevices:(NSArray<id<SBMConnectableDevice>> *)devices;

/**
 Method used to get node that represents given device.

 @param device Device for which to retrieve node object.
 @return Node object, or nil if no node is found for given device.
 */
+ (nullable SBMNode *)node:(id<SBMConnectableDevice>)device;

/**
 Method used to get node that represents given device in specific subnet.

 @param device Device for which to retrieve node object.
 @param subnet Subnet in which to look for node object.
 @return Node object, or nil if no node is found for given device in subnet.
 */
+ (nullable SBMNode *)node:(id<SBMConnectableDevice>)device inSubnet:(SBMSubnet*)subnet;

/**
 Method used to get all subnets for given device.

 @param device Device for which to retrieve subnets.
 @return Array of subnets that are found for given device.
 */
+ (NSArray<SBMSubnet *> *)allSubnets:(id<SBMConnectableDevice>)device;

@end

NS_ASSUME_NONNULL_END
