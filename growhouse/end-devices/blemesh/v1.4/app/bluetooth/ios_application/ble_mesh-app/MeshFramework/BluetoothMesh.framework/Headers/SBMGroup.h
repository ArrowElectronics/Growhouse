//
//  SBMGroup.h
//  ble_mesh-lib
//
//  Created by Michal Lenart on 24/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMApplicationKey.h"
#import "SBMNode.h"
#import "SBMSigModel.h"
#import "SBMVendorModel.h"
#import "SBMGroupRemoval.h"
#import "SBMBluetoothMeshMacros.h"

/**
 A SBMGroup is the object that contains an Application Key and a publish and subscribe addresses that will be
 used to configure the models inside a SBMNode. It is abstract part of Bluetooth Mesh not included in official Bluetooth Mesh Specification.
 */

NS_ASSUME_NONNULL_BEGIN

/**
 SBMGroup object that contains information about mesh group.
 */
@interface SBMGroup : NSObject
/**
 Group name.
 */
@property (nonatomic, readwrite) NSString *name;

/**
 Group application key.
 */
@property (nonatomic, readonly) SBMApplicationKey *appKey;

/**
 Group nodes.
 */
@property (nonatomic, readonly) NSArray<SBMNode *> *nodes;

/**
 Group SIG Models.
 */
@property (nonatomic, readonly) NSArray<SBMSigModel *> *sigModels;

/**
 Group Vendor Models.
 */
@property (nonatomic, readonly) NSArray<SBMVendorModel *> *vendorModels;

/**
 Subnet that group is contained in.
 */
@property (nonatomic, readonly, weak) SBMSubnet *subnet;

/**
 Group address.
 */
@property (nonatomic, readonly) UInt16 address;

/**
 Address to publish message to Group. This information is only for read and from the application perspective should be not used for anything more. Used in framework logic.
 */
@property (nonatomic, readonly) UInt16 publicationAddress BLUETOOTH_MESH_API_DEPRECATED_USE_INSTEAD(2.1.0, "Address");

/**
 Address to subscribe to Group. This information is only for read and from the application perspective should be not used for anything more. Used in framework logic.
 */
@property (nonatomic, readonly) UInt16 subscriptionAddress BLUETOOTH_MESH_API_DEPRECATED_USE_INSTEAD(2.1.0, "Address");

/**
 Method to remove group.

 @param errorCallback Action invoked on group removal.
 */
- (void)removeGroupWithCallback:(SBMGroupRemovalSuccess)successCallback errorCallback:(SBMGroupRemovalError)errorCallback;

/**
 Removes group only from local storage.
 */
- (void)removeOnlyFromLocalStructure;

/**
 Not available.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available.
 */
- (instancetype)init NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
