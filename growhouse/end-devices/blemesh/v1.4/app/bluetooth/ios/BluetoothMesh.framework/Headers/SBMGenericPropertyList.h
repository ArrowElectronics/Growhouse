//
//  SBMGenericPropertyList.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 26/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import "SBMControlValueGetSigModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Base class for Generic PropertyList functionality objects.
 */
@interface SBMGenericPropertyList : NSObject<SBMControlValueGetSigModel>

/**
 Property list buffer.
 */
@property (nonatomic, readonly) NSData* buffer;

/**
 Initialize new generic property list object.

 @param buffer Property list buffer.
 @return An initialized generic property list object, or nil if an object could not be created.
 */
- (instancetype)initWithBuffer:(NSData*)buffer;

/**
 Not available. Use initWithBuffer instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithBuffer instead.
 */
- (instancetype)init NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
