//
//  SBMVendorModelNotifications.h
//  BluetoothMesh
//
//  Created by Łukasz Rzepka on 21/03/2019.
//  Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMVendorModel.h"
#import "SBMGroup.h"
#import "SBMNode.h"

NS_ASSUME_NONNULL_BEGIN

@interface SBMVendorModelNotifications : NSObject

/**
 @brief Subscribed to address from group.
 */
@property (nonatomic, readonly, nullable) SBMGroup *group;

/**
 @brief Subscribed to address from node.
 */
@property (nonatomic, readonly, nullable) SBMNode *node;

/**
 @brief Initialize new object with group.
 
 @param group SBMGroup to which subscription will be set.
 @return An initialized object, or nil if an object could not be created.
 */
- (instancetype)initWithGroup:(SBMGroup * const)group;

/**
 @brief Initialize new object with node.
 
 @param node SBMNode to which subscription will be set.
 @return An initialized object, or nil if an object could not be created.
 */
- (instancetype)initWithNode:(SBMNode * const)node;

/**
 Not available. Use initWithNode or initWithGroup instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use Use initWithNode or initWithGroup instead.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 @brief Method used to subscribe to vendor model notifications.
 
 Status updates will come from the SBMLocalVendorSettings
 which should be configured by SBMLocalVendorRegisterControl before.
 
 @param vendorModel SBMVendorModel which status should be subscribed to.
 @return nil if subscribe with success, error otherwise.
 */
- (nullable NSError *)signUpForNotifications:(SBMVendorModel *)vendorModel;

/**
 @brief Method used to unsubscribe from vendor model notifications.
 
 @param vendorModel SBMVendorModel which status should be unsubscribed from.
 @return nil if unsubscribe with success, error otherwise.
 */
- (nullable NSError *)signOutFromNotifications:(SBMVendorModel *)vendorModel;

@end

NS_ASSUME_NONNULL_END
