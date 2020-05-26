//
//  SBMVendorSubscriptionControl.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 08/12/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "SBMVendorModel.h"
#import "SBMPublicationSettings.h"
#import "SBMVendorRegistrationSettings.h"
#import "SBMBluetoothMeshMacros.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Object used to get/set registration and publication settings of a vendor model.
 */
BLUETOOTH_MESH_API_DEPRECATED_USE_INSTEAD(2.1.0, "SBMLocalVendorRegisterControl")
@interface SBMVendorSubscriptionControl : NSObject

/**
 Callback for successfull operation on vendor settings.

 @param subscriptionControl Subscription control object used for operation.
 */
typedef void (^SBMVendorSubscriptionControlSuccess)(SBMVendorSubscriptionControl *subscriptionControl) BLUETOOTH_MESH_API_DEPRECATED_USE_INSTEAD(2.1.0, "SBMLocalVendorRegisterControl");

/**
 Callback for getting publication settings of a vendor model.

 @param subscriptionControl Subscription control object used for operation.
 @param settings Vendor model publication settings.
 */
typedef void (^SBMVendorSubscriptionControlGetPublicationSettings)(SBMVendorSubscriptionControl *subscriptionControl, SBMPublicationSettings *settings) BLUETOOTH_MESH_API_DEPRECATED(2.1.0);

/**
 Callback for checking whether a model is registered.

 @param subscriptionControl Subscription control object used for operation.
 @param isRegistered A Boolean value that determines whether model is registered.
 */
typedef void (^SBMVendorSubscriptionControlIsRegistered)(SBMVendorSubscriptionControl *subscriptionControl, BOOL isRegistered) BLUETOOTH_MESH_API_DEPRECATED_USE_INSTEAD(2.1.0, "SBMLocalVendorRegisterControl");

/**
 Callback for failed operation on vendor settings.

 @param subscriptionControl Subscription control object used for operation.
 @param error Error describing reason of failure.
 */
typedef void (^SBMVendorSubscriptionControlError)(SBMVendorSubscriptionControl *subscriptionControl, NSError * _Nullable error) BLUETOOTH_MESH_API_DEPRECATED_USE_INSTEAD(2.1.0, "SBMLocalVendorRegisterControl");

/**
 Initialize new vendor subscription control object.

 @param model Model which will be controlled.
 @return Initialized vendor subscription control object, or nil if an object could not be created for some reason that would not result in an exception.
 */
- (instancetype)initWithModel:(SBMVendorModel *)model  BLUETOOTH_MESH_API_DEPRECATED_USE_INSTEAD(2.1.0, "SBMLocalVendorRegisterControl");

/**
 Not available. Use initWithModel instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithModel instead.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 Gets publication settings of a vendor model.

 @param successCallback Action invoked when operation succeeds.
 @param errorCallback Action invoked when operation fails.
 */
- (void)publicationSettings:(SBMVendorSubscriptionControlGetPublicationSettings)successCallback
              errorCallback:(SBMVendorSubscriptionControlError)errorCallback BLUETOOTH_MESH_API_DEPRECATED(2.1.0);

/**
 Check whether a model is registered.

 @param callback Action invoked on completion.
 */
- (void)isRegistered:(SBMVendorSubscriptionControlIsRegistered)callback BLUETOOTH_MESH_API_DEPRECATED_USE_INSTEAD(2.1.0, "SBMLocalVendorRegisterControl");

/**
 Registers a vendor model.

 @param settings Vendor registration settings.
 @param successCallback Action invoked when operation succeeds.
 @param errorCallback Action invoked when operation fails.
 */
- (void)registerSettings:(SBMVendorRegistrationSettings *)settings
         successCallback:(SBMVendorSubscriptionControlSuccess)successCallback
           errorCallback:(SBMVendorSubscriptionControlError)errorCallback BLUETOOTH_MESH_API_DEPRECATED_USE_INSTEAD(2.1.0, "SBMLocalVendorRegisterControl");

/**
 Unregisters a vendor model.

 @param successCallback Action invoked when operation succeeds.
 @param errorCallback Action invoked when operation fails.
 */
- (void)unregister:(SBMVendorSubscriptionControlSuccess)successCallback
     errorCallback:(SBMVendorSubscriptionControlError)errorCallback BLUETOOTH_MESH_API_DEPRECATED_USE_INSTEAD(2.1.0, "SBMLocalVendorRegisterControl");

@end

NS_ASSUME_NONNULL_END
