//
//  SBMGenericProperty.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 23/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMControlValueGetSigModel.h"
#import "SBMControlValueSetSigModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Base class for Generic Property functionality objects.
 */
@interface SBMGenericProperty : NSObject<SBMControlValueGetSigModel, SBMControlValueSetSigModel>

/**
 Value that represents a device property. A value of 0 is prohibited.
 */
@property (nonatomic, readonly) UInt16 identifier;

/**
 Value that indicates whether the device property can be read or written as a Generic UserProperty.
 
  - Values of 0 and 4 - 255 are prohibited.
 
  - A value of 1 means that device property can be read.
 
  - A value of 2 means that device property can be written.
 
  - A value of 3 means that device property can be read and written.
 */
@property (nonatomic, readonly) UInt8 access;

/**
 Data containing property value.
 */
@property (nonatomic, readonly) NSData* buffer;

/**
 Initialize new generic property object.

 @param identifier Device property identifier.
 @param access Device property access level.
 @param buffer Device property value.
 @return An initialized generic property object, or nil if an object could not be created.
 */
- (instancetype)initWithIdentifier:(UInt16)identifier access:(UInt8)access buffer:(NSData*)buffer;

/**
 Not available. Use initWithIdentifier instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithIdentifier instead.
 */
- (instancetype)init NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
