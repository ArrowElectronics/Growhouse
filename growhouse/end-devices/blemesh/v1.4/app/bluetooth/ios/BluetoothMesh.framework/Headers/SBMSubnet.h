//
//  SBMSubnet.h
//  BluetoothMesh
//
//  Created by Arkadiusz Marszałek on 13/11/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMGroup.h"
#import "SBMNode.h"
#import "SBMNetworkKey.h"
#import "SBMTuple.h"
#import "SBMSubnetRemoval.h"
@class SBMNetwork;

NS_ASSUME_NONNULL_BEGIN

/**
 A SBMSubnet object contains informations about the Bluetooth Mesh Network.
 */
@interface SBMSubnet : NSObject

/**
 Boolean property that determines whether subnet is primary in network.
 */
@property (nonatomic, readonly) BOOL primary;

/**
 Subnet name.
 */
@property (nonatomic, readwrite) NSString* name;

/**
 Subnet key.
 */
@property (nonatomic, readonly) SBMNetworkKey* netKey;

/**
 Subnet's network.
 */
@property (nonatomic, readonly, weak) SBMNetwork* network;

/**
 Subnet's groups.
 */
@property (nonatomic, readonly) NSArray<SBMGroup*>* groups;

/**
 Subnet's nodes.
 */
@property (nonatomic, readonly) NSArray<SBMNode*>* nodes;

/**
 Checks if new Group object can be created.
 */
- (BOOL)canCreateGroup;

/**
 Method to create new group in subnet.
 If can return nil in case of max number of groups is reached. See canCreateGroup.

 @param groupName Name of the group to create.
 @return Created group.
 */
- (SBMGroup*)createGroupWithName:(NSString*)groupName;

/**
 Method to remove subnet.

 @param successCallback Action invoked on subnet removal succeed.
 @param errorCallback Action invoked on subnet removal failed.
 */
- (void)removeSubnetWithCallback:(SBMSubnetRemovalSuccess)successCallback errorCallback:(SBMSubnetRemovalError)errorCallback;

/**
 Removes subnet only from local storage.
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
