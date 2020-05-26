//
//  SBMLocalFunctionalityBinder.h
//  BluetoothMesh
//
//  Created by Łukasz Rzepka on 26/03/2019.
//  Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMApplicationKey.h"
#import "SBMLocalVendorModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Local Vendor Cryptography binder object. It is used to bind a local vendor models with application keys.
 */

@interface SBMLocalVendorCryptoBinder : NSObject

/**
 Initialize new Local Vendor Cryptography object.
 
 @param applicationKey SBMApplicationKey to which local vendor models will be bound. SBMApplicationKey comes from the SBMGroup.
 @return An initialized Local Vendor Cryptography object, or nil if an object could not be created.
 */
- (instancetype)initWithApplicationKey:(SBMApplicationKey *)applicationKey;

/**
 Not available. Use initWithApplicationKey instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithApplicationKey instead.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 @brief Bind an application key to a local vendor model.
 
 For a local vendor model to receive an incoming message, the message must be
 decrypted using an application key which is bound to the vendor model on the node side.
 This function creates binding between a local vendor model and a needed application key
 to decrypt this message.
 
 Common error: 3073
 Returned when trying to add binding which already exists
 
 @param localVendorModel SBMLocalVendorModel to bind with key.
 */
- (nullable NSError *)bindApplicationKeyTo:(SBMLocalVendorModel *)localVendorModel;

/**
 @brief Remove an application key binding with a local vendor model.
 
 Remove an existing binding between a local vendor model and an application key,
 meaning that the local vendor model will no longer process messages encrypted
 using that key.
 
 @param localVendorModel SBMLocalVendorModel to unbind from key.
 */
- (nullable NSError *)unbindApplicationKeyFrom:(SBMLocalVendorModel *)localVendorModel;

@end

NS_ASSUME_NONNULL_END
