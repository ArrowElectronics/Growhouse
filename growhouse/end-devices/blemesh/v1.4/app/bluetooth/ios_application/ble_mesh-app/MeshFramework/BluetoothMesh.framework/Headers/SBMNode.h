//
//  SBMNode.h
//  ble_mesh-lib
//
//  Created by Michal Lenart on 23/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMDeviceCompositionData.h"
#import "SBMDeviceKey.h"
#import "SBMElement.h"

@class SBMNodeControl;
@class SBMSubnet;
@class SBMGroup;

NS_ASSUME_NONNULL_BEGIN

/**
 A SBMNode is the object that contains all the information about a Bluetooth Mesh device in a Network. It is created right after provision a device. After add node to group (Configure available models from DCD), it will be possible to get all configured Elements and Models from the configuration object. DCD can be also downloaded manually without adding to the specific group.
 */
@interface SBMNode : NSObject

/**
 Node name.
 */
@property (nonatomic, readwrite) NSString *name;

/**
 Mesh device primary element address
 */
@property (nonatomic, readonly) NSInteger primaryElementAddress;

/**
 Mesh device UUID taken from the Bluetooth device advertisement.
 */
@property (nonatomic, readonly) NSData *uuid;

/**
 Device crypto key.
 */
@property (nonatomic, readonly) SBMDeviceKey *devKey;

/**
 Array of groups that this node is in.
 */
@property (nonatomic, readonly) NSArray<SBMGroup *> *groups;

/**
 Array of subnets that this node is in.
 */
@property (nonatomic, readonly) NSArray<SBMSubnet *> *subnets;

/**
 Node elements.
 */
@property (nonatomic, readonly) NSArray<SBMElement *> *elements;

/**
 Node device composition data.
 */
@property (nonatomic, readonly, nullable) SBMDeviceCompositionData *deviceCompositionData;

/**
 Not available.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 Check whether node is connected to the network as proxy.

 @return Boolean value specifying whether node is connected to the network as proxy.
 */
- (BOOL)isConnectedAsProxy;

/**
 Removes node only from local storage.
 */
- (void)removeOnlyFromLocalStructure;

@end

NS_ASSUME_NONNULL_END
