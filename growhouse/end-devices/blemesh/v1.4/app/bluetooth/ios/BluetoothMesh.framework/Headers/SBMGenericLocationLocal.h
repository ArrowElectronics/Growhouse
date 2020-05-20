//
//  SBMGenericLocationLocal.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 23/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMControlValueGetSigModel.h"
#import "SBMControlValueSetSigModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Generic LocationLocal functionality object. It represents location information of an elements: local north, local east, local altitude, floor number, uncertainty.
 */
@interface SBMGenericLocationLocal : NSObject<SBMControlValueGetSigModel, SBMControlValueSetSigModel>

/**
 Value of Generic LocationLocal northSouthOffset. It describes the North coordinate of the device using a local coordinate system. The value of 0x8000 means the information is not configured.
 */
@property (nonatomic, readonly) SInt16 northSouthOffset;

/**
 Value of Generic LocationLocal eastWestOffset. It describes the East coordinate of the device using a local coordinate system. The value of 0x8000 means the information is not configured.
 */
@property (nonatomic, readonly) SInt16 eastWestOffset;

/**
 Value of Generic LocationLocal altitude. It describes the altitude of the device relative to the Generic LocationGlobal Altitude. The value of 0x7FFF indicates that the Local altitude is not configured. The value of 0x7FFE indicates that the Local altitude is greater than or equal to 32766 decimeters. The value of 0x8000 - 0x7FFD indicates that the Global altitude is from -32768 decimeters through 32765 decimeters.
 */
@property (nonatomic, readonly) SInt16 altitude;

/**
 Value of Generic LocationLocal floor. It describes the floor number where the element is installed. The value of 0x00 indicates that the floor is -20 or below. The value of 0x01 - 0xFB indicates that the floor is N + 20. The value of 0xFC indicates that the floor is 232 or above. The value of 0xFD indicates ground floor (0). The value of 0xFE indicates ground floor (1). The value of 0xFF indicates that the Local floor is not configured.
 */
@property (nonatomic, readonly) UInt8 floor;

/**
 Value of Generic LocationLocal uncertainty. It describes the uncertainty of the location information the element exposes.
 */
@property (nonatomic, readonly) UInt16 uncertainty;

/**
 Initialize new Generic LocationLocal functionality object.

 @param northSouthOffset NorthSouthOffset value that functionality should be initialized with.
 @param eastWestOffset EastWestOffset value that functionality should be initialized with.
 @param altitude Altitude value that functionality should be initialized with.
 @param floor Floor value that functionality should be initialized with.
 @param uncertainty Uncertainty value that functionality should be initialized with.
 @return An initialized Generic LocationLocal functionality object, or nil if an object could not be created.
 */
- (instancetype)initWithNorthSouthOffset:(SInt16)northSouthOffset
                          eastWestOffset:(SInt16)eastWestOffset
                                altitude:(SInt16)altitude
                                   floor:(UInt8)floor
                             uncertainty:(UInt16)uncertainty;

/**
 Not available. Use initWithNorthSouthOffset instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithNorthSouthOffset instead.
 */
- (instancetype)init NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
