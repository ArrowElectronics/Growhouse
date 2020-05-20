//
//  SBMApplicationKey.h
//  ble_mesh-lib
//
//  Created by Lukasz Rzepka on 04.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 It is a container for application key. Application key is used to secure and authenticate messages at the access layer. Each SBMGroup has own application key.
*/
@interface SBMApplicationKey : NSObject

/**
 Current value of application key data.
 */
@property (nonatomic, readonly) NSData * rawKey;

/**
 Old value of application key data.
 */
@property (nonatomic, readonly, nullable) NSData * oldRawKey;

/**
 Index of application key.
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
