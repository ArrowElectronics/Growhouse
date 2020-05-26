//
//  SBMNetworkKey.h
//  ble_mesh-lib
//
//  Created by Lukasz Rzepka on 04.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 Container for network key. Each subnet has its own network key.
 */
@interface SBMNetworkKey : NSObject

/**
 Current value of network key data.
 */
@property (nonatomic, readonly) NSData * rawKey;

/**
 Old value of network key data.
 */
@property (nonatomic, readonly, nullable) NSData * oldRawKey;

/**
 Key is a network key index for network key from the mesh.
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
