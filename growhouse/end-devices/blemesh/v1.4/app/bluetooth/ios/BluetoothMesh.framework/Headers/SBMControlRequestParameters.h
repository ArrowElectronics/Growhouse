//
//  SBMRequestParameters.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 25/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 Object used to specify parameters used when sending control request.
 */
@interface SBMControlRequestParameters : NSObject

/**
 Transition time in milliseconds, or zero for immediate change request.
 */
@property (nonatomic, readonly) UInt16 transitionTime;

/**
 Delay in milliseconds before the server acts on the request, or zero for immediate action.
 */
@property (nonatomic, readonly) UInt16 delayTime;

/**
 A boolean value that determines whether an explicit response (status message) is required.
 */
@property (nonatomic, readonly) BOOL requestReply;

/**
 Initialize new control request parameters object.

 @param transitionTime Transition time in milliseconds from one value to other.
 @param delayTime Delay time in milliseconds before the server acts on the request.
 @param requestReply A boolean value that determines whether an explicit response (status message) is required.
 @return An initialized control request parameters object, or nil if an object could not be created.
 */
- (instancetype)initWithTransitionTime:(UInt16)transitionTime delayTime:(UInt16)delayTime requestReplay:(BOOL)requestReply;

/**
 Not available. Use initWithTransitionTime instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithTransitionTime instead.
 */
- (instancetype)init NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
