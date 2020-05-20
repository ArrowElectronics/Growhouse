//
//  SBMNodeControl.h
//  BluetoothMesh
//
//  Created by Arkadiusz Marszałek on 27/11/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMNode.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Callback for successfull binding operation.
 */
typedef void(^SBMNodeControlBindGroupSuccess)(void);

/**
 Callback for failed binding operation.

 @param error Error describing reason of failure.
 */
typedef void(^SBMNodeControlBindGroupError)(NSError* error);

/**
 Callback for successfull unbinding operation.
 */
typedef void(^SBMNodeControlUnbindGroupSuccess)(void);

/**
 Callback for failed unbinding operation.

 @param error Error describing reason of failure.
 */
typedef void(^SBMNodeControlUnbindGroupError)(NSError* error);

/**
 Callback for successfull add to subnet operation.
 */
typedef void(^SBMNodeControlAddToSubnetSuccess)(void);

/**
 Callback for failed add to subnet operation.

 @param error Error describing reason of failure.
 */
typedef void(^SBMNodeControlAddToSubnetError)(NSError* error);

/**
 Callback for successfull remove from subnet operation.
 */
typedef void(^SBMNodeControlRemoveFromSubnetSuccess)(void);

/**
 Callback for failed remove from subnet operation.

 @param error Error describing reason of failure.
 */
typedef void(^SBMNodeControlRemoveFromSubnetError)(NSError* error);

/**
 Object used to bind node to group and unbind node from group.
 */
@interface SBMNodeControl : NSObject

/**
 Node that is controlled and used for bind/unbind operations.
 */
@property (nonatomic, readonly) SBMNode* node;

/**
 Initialize new node control object.

 @param node Node which will be controlled and used for bind/unbind operations.
 @return Initialized node control object, or nil if an object could not be created for some reason that would not result in an exception.
 */
- (instancetype)initWithNode:(SBMNode*)node;

/**
 Not available. Use initWithNode instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithNode instead.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 Binds node to specified group.

 @param group Group to bind node to.
 @param successCallback Action invoked when operation succeeds.
 @param errorCallback Action invoked when operation fails.
 */
- (void)bindToGroup:(SBMGroup*)group successCallback:(SBMNodeControlBindGroupSuccess)successCallback errorCallback:(SBMNodeControlBindGroupError)errorCallback;

/**
 Unbinds node from specified group.

 @param group Group to unbind node from.
 @param successCallback Action invoked when operation succeeds.
 @param errorCallback Action invoked when operation fails.
 */
- (void)unbindFromGroup:(SBMGroup*)group successCallback:(SBMNodeControlUnbindGroupSuccess)successCallback errorCallback:(SBMNodeControlUnbindGroupError)errorCallback;

/**
 Adds node to specified subnet.

 @param subnet Subnet to add node to.
 @param successCallback Action invoked when operation succeeds.
 @param errorCallback Action invoked when operation fails.
 */
- (void)addToSubnet:(SBMSubnet*)subnet successCallback:(SBMNodeControlAddToSubnetSuccess)successCallback errorCallback:(SBMNodeControlAddToSubnetError)errorCallback;

/**
 Removes node from specified subnet.

 @param subnet Subnet to remove node from.
 @param successCallback Action invoked when operation succeeds.
 @param errorCallback Action invoked when operation fails.
 */
- (void)removeFromSubnet:(SBMSubnet*)subnet successCallback:(SBMNodeControlRemoveFromSubnetSuccess)successCallback errorCallback:(SBMNodeControlRemoveFromSubnetError)errorCallback;

@end

NS_ASSUME_NONNULL_END
