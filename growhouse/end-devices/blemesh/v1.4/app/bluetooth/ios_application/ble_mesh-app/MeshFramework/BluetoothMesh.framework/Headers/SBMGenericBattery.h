//
//  SBMGenericBattery.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 26/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMControlValueGetSigModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Possible states of Generic Battery Presence.
 */
typedef NS_ENUM(NSUInteger, SBMGenericBatteryFlagsPresenceState) {
    /** The battery is not present. */
    SBMGenericBatteryFlagsPresenceStateNotPresent,
    /** The battery is present and is removable. */
    SBMGenericBatteryFlagsPresenceStateRemovable,
    /** The battery is present and is non-removable. */
    SBMGenericBatteryFlagsPresenceStateNonRemovable,
    /**The battery presence is unknown.*/
    SBMGenericBatteryFlagsPresenceStateUnknown
};

/**
 Possible states of Generic Battery Indicator.
 */
typedef NS_ENUM(NSUInteger, SBMGenericBatteryFlagsIndicatorState) {
    /** The battery charge is at critically low level. */
    SBMGenericBatteryFlagsIndicatorStateCriticallyLow,
    /** The battery charge is at low level. */
    SBMGenericBatteryFlagsIndicatorStateLow,
    /** The battery charge is at good level. */
    SBMGenericBatteryFlagsIndicatorStateGood,
    /** The battery charge is unknown. */
    SBMGenericBatteryFlagsIndicatorStateUnknown
};

/**
 Possible states of Generic Battery Charging.
 */
typedef NS_ENUM(NSUInteger, SBMGenericBatteryFlagsChargingState) {
    /** The battery is not chargeable. */
    SBMGenericBatteryFlagsChargingStateNotChargeable,
    /** The battery is chargeable and is not charging. */
    SBMGenericBatteryFlagsChargingStateNotCharging,
    /** The battery is chargeable and is charging. */
    SBMGenericBatteryFlagsChargingStateCharging,
    /** The battery charging state is unknown. */
    SBMGenericBatteryFlagsChargingStateUnknown
};

/**
 Possible states of Generic Battery Serviceability.
 */
typedef NS_ENUM(NSUInteger, SBMGenericBatteryFlagsServiceabilityState) {
    /** Reserved for future use. */
    SBMGenericBatteryFlagsServiceabilityStateReserved,
    /** The battery does not require service. */
    SBMGenericBatteryFlagsServiceabilityStateServiceNotRequired,
    /** The battery requires service. */
    SBMGenericBatteryFlagsServiceabilityStateServiceRequired,
    /** The battery serviceability is unknown. */
    SBMGenericBatteryFlagsServiceabilityStateUnknown
};

/**
 Generic Battery functionality object. It represents the state of a battery: charge level, remaining time to complete discharging, remaining time to complete charging, and flags.
 */
@interface SBMGenericBattery : NSObject<SBMControlValueGetSigModel>

/**
 Value of Generic Battery level. It represents battery charge level in percents. A value of 0xFF means that charge level is unknown. Values of 0x65 to 0xFE are prohibited.
 */
@property (nonatomic, readonly) UInt8 level;

/**
 Value of Generic Battery discharge time. It represents the remaining time of the discharging process in minutes. A value of 0xFFFFFF means that discharge time is unknown.
 */
@property (nonatomic, readonly) UInt32 dischargeTime;

/**
 Value of Generic Battery charge time. It represents the remaining time of the charging process in minutes. A value of 0xFFFFFF means that charge time is unknown.
 */
@property (nonatomic, readonly) UInt32 chargeTime;

/**
 Value of Generic Battery presence state. It indicates the presence of a battery.
 */
@property (nonatomic, readonly) SBMGenericBatteryFlagsPresenceState presenceState;

/**
 Value of Generic Battery inidicator state. It indicates the charge level of a battery.
 */
@property (nonatomic, readonly) SBMGenericBatteryFlagsIndicatorState indicatorState;

/**
 Value of Generic Battery charging state. It indicates whether a battery is charging.
 */
@property (nonatomic, readonly) SBMGenericBatteryFlagsChargingState chargingState;

/**
 Value of Generic Battery serviceability state. It indicates the serviceability of a battery.
 */
@property (nonatomic, readonly) SBMGenericBatteryFlagsServiceabilityState serviceabilityState;

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
