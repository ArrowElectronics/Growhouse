//
//  SBMProvisionerParameters.h
//  BluetoothMesh
//
//  Created by Arkadiusz Marszałek on 12/03/2019.
//  Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 Object that contains parameters for device to be provisioned.
 */
@interface SBMProvisionerParameters : NSObject

/**
 Address to assign for the provisioned device.
 */
@property (nonatomic, readonly) UInt16 address;

/**
 Number of elements the device to be provisioned contains.
 */
@property (nonatomic, readonly) UInt8 elements;

/**
 Attention timer for provisioned device.
 */
@property (nonatomic, readonly) UInt8 attentionTimer;

/**
 Initializes new provisioner parameters object.

 @param address Address to assign for the provisioned device.
 @param elements Number of elements the device to be provisioned contains.
 @param attentionTimer Attention timer for provisioned device.
 @return Initialized provisioner parameters object, or nil if an object could not be created for some reason that would not result in an exception.
 */
- (instancetype)initWithAddress:(UInt16)address elements:(UInt8)elements attentionTimer:(UInt8)attentionTimer;

@end

NS_ASSUME_NONNULL_END
