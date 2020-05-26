//
//  SBMGenericLocationGlobal.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 23/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMControlValueGetSigModel.h"
#import "SBMControlValueSetSigModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Generic LocationGlobal functionality object. It represents location information of an elements: latitude, longitude, altitude.
 */
@interface SBMGenericLocationGlobal : NSObject<SBMControlValueGetSigModel, SBMControlValueSetSigModel>

/**
 Value of Generic LocationGlobal latitude. It represents the global WGS84 North coordinate of the element. The value of 0x80000000 indicates that the Global latitude is not configured.
 */
@property (nonatomic, readonly) SInt32 latitude;

/**
 Value of Generic LocationGlobal longitude. It represents the global WGS84 East coordinate of the element. The value of 0x80000000 indicates that the Global longitude is not configured.
 */
@property (nonatomic, readonly) SInt32 longitude;

/**
 Value of Generic LocationGlobal altitude. It represents the altitude of the device above the WGS84 datum. The value of 0x7FFF indicates that the Global altitude is not configured. The value of 0x7FFE indicates that the Global altitude is greater than or equal to 32766 meters. The value of 0x8000 - 0x7FFD indicates that the Global altitude is from -32768 meters through 32765 meters.
 */
@property (nonatomic, readonly) SInt16 altitude;

/**
 Initialize new Generic LocationGlobal functionality object.

 @param latitude Latitude value that functionality should be initialized with.
 @param longitude Longitude value that functionality should be initialized with.
 @param altitude Altitude value that functionality should be initialized with.
 @return An initialized Generic LocationGlobal functionality object, or nil if an object could not be created.
 */
- (instancetype)initWithLatitude:(SInt32)latitude longitude:(SInt32)longitude altitude:(SInt16)altitude;

/**
 Not available. Use initWithLatitude instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithLatitude instead.
 */
- (instancetype)init NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
