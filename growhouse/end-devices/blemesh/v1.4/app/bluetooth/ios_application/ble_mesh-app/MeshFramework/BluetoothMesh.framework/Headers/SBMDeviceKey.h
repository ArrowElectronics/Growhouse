//
//  SBMDeviceKey.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 29/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 It is a container for device key. Each SBMNode has own device key.
 */
@interface SBMDeviceKey : NSObject

/**
 Raw data of device crypto key.
 
 *  *  *  *  *
 
 * UUID of the device:
 
 
 ````
 a = object for "kCBAdvDataServiceData" key from advertisement data.
 b = data for CBUUID: 1827 key from the 'a'
 UUID = data from (0,16) range from the 'b'
 ````
 
 */
@property (nonatomic, readonly) NSData * rawKey;

/**
 Device key index for device key in mesh.
 */
@property (nonatomic, readonly) NSInteger keyIndex;

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
