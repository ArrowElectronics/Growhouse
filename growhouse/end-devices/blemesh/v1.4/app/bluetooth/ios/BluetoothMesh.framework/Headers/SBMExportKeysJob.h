//
//  SBMExportKeys.h
//  BluetoothMesh
//
//  Created by Lukasz Rzepka on 19/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMBluetoothMesh.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Object used for exporting keys from bluetooth mesh.
 */
@interface SBMExportKeysJob : NSObject

/**
 Not available. Use initWithBluetoothMesh instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithBluetoothMesh instead.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 Initialize export keys job object used to export keys from bluetooth mesh.

 @param mesh Bluetooth Mesh object from which the keys will be exported.
 @return Initialized export keys job object, or nil if an object could not be created for some reason that would not result in an exception.
 */
- (instancetype)initWithBluetoothMesh:(SBMBluetoothMesh*)mesh;

/**
 Exports keys from bluetooth mesh.

 @return Dictionary of exported keys.
 */
- (NSDictionary*)fire;

@end

NS_ASSUME_NONNULL_END
