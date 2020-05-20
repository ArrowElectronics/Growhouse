//
//  SBMLocalVendorRegistrator.h
//  BluetoothMesh
//
//  Created by Lukasz Rzepka on 20/03/2019.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "SBMLocalVendorModel.h"
#import "SBMLocalVendorSettings.h"

NS_ASSUME_NONNULL_BEGIN

/**
 @brief Object used to get/set registration and publication settings of a vendor model.
 */
@interface SBMLocalVendorRegistrator : NSObject

/**
 @brief Initialize new vendor register control object.

 @param model Model which will be controlled.
 @return Initialized vendor register control object, or nil if an object could not be created for some reason that would not result in an exception.
 */
- (instancetype)initWithModel:(SBMLocalVendorModel *)model;

/**
 @brief Not available. Use initWithModel instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 @brief Not available. Use initWithModel instead.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 @brief Check whether a local vendor model is registered.
 
 Check whether a model is registered

 @return Either the model is registered.
 */
- (BOOL)isRegistered;

/**
 @brief Registers a local vendor model.
 
 Registering a vendor model hooks it to the mesh stack and
 enables the stack to pass incoming messages to the model.
 Typically registration should be done one time for a one SBMLocalVendorModel.
 
 ------
 
 Use SBMControlElement:setStatus:successCallback:errorCallback: to send value to vendor model
 and if vendor model support reply to a message, reply will come through SBMLocalVendorSettingsMessageHandler
 from the SBMLocalVendorSettings.
 
 ------
 
 Use SBMVendorModelNotifications to subscribe to vendor model notifications
 from the mesh network. Notifications will come through SBMLocalVendorSettingsMessageHandler
 from the SBMLocalVendorSettings.
 
 ------

 @param settings Vendor registration settings.
 @return nil if register settings with success, error otherwise.
 */
- (nullable NSError *)registerSettings:(SBMLocalVendorSettings *)settings;

/**
 @brief Unregisters a local vendor model.
 
 Unregistering a vendor model decouples it from the mesh stack,
 meaning that the stack will no longer call the model callbacks
 when messages are received.

 @return nil if register settings with success, error otherwise.
 */
- (nullable NSError *)unregister;

@end

NS_ASSUME_NONNULL_END
