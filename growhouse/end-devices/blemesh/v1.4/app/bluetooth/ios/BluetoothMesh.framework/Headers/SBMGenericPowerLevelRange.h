//
//  SBMGenericPowerLevelRange.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 23/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMControlValueGetSigModel.h"
#import "SBMControlValueSetSigModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Possible states of GenericPowerLevelRange states.
 */
typedef NS_ENUM(NSInteger, SBMGenericPowerLevelRangeState) {
    /** Command successfully processed. */
    SBMGenericPowerLevelRangeStateSuccess,
    /** The provided value for range min cannot be set. */
    SBMGenericPowerLevelRangeStateCannotSetMin,
    /** The provided value for range max cannot be set. */
    SBMGenericPowerLevelRangeStateCannotSetMax,
    /** Reserved for future use. */
    SBMGenericPowerLevelRangeStateReserved
};

/**
 Generic PowerLevelDefault functionality object. It represents the minimum and maximum power levels of and element.
 */
@interface SBMGenericPowerLevelRange : NSObject<SBMControlValueGetSigModel, SBMControlValueSetSigModel>

/**
 Value of Generic PowerLevelRange functionality minimum. 0 is prohibited.
 */
@property (nonatomic, readonly) UInt16 min;

/**
 Value of Generic PowerLevelRange functionality maximum. 0 is prohibited.
 */
@property (nonatomic, readonly) UInt16 max;

/**
 Status for the last operation on the GenericPowerRange state.
 */
@property (nonatomic, readonly) SBMGenericPowerLevelRangeState status;

/**
 Initialize new Generic PowerLevelRange functionality object.

 @param min PowerLevelRange minimum value that functionality should be initialized with.
 @param max PowerLevelRange maximum value that functionality should be initialized with.
 @return An initialized Generic PowerLevelRange functionality object, or nil if an object could not be created.
 */
- (instancetype)initWithMin:(UInt16)min max:(UInt16)max;

/**
 Not available. Use initWithMin instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithMin instead.
 */
- (instancetype)init NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
