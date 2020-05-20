//
//  SBMKeyRefresh.h
//  ble_mesh-lib
//
//  Created by Arkadiusz Marszałek on 29.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import "SBMSubnet.h"

/**
 Action invoked on completed key refresh procedure.

 @param success A boolean value that determines whether key refresh procedure succeeded.
 */
typedef void(^KeyRefreshCallback)(BOOL success);

/**
 Object used to refresh keys in network.
 */
@interface SBMKeyRefresh : NSObject

/**
 A boolean value that determines whether key refresh procedure is started.
 */
@property (nonatomic, readonly) BOOL isStarted;

/**
 Subnet that key refresh procedure is performed on.
 */
@property (nonatomic, strong, readonly) SBMSubnet *subnet;

/**
 Action called on key refresh procedure completion.
 */
@property (nonatomic, strong, readonly) KeyRefreshCallback callback;

/**
 Array of groups for which keys should be refreshed.
 */
@property (nonatomic, strong) NSArray<SBMGroup*> *groups;

/**
 Array that contains blacklisted nodes. Set node as blacklisted to effectively exclude it from key refresh procedure.
 */
@property (nonatomic, strong) NSArray<SBMNode*> *blacklist;

/**
 Initialize key refresh object used to refresh keys in network.

 @param subnet Subnet that key refresh procedure is performed on.
 @param callback Callback called on key refresh procedure completion.
 @return Initialized key refresh object, or nil if an object could not be created for some reason that would not result in an exception.
 */
- (instancetype)initWithSubnet:(SBMSubnet*)subnet keyRefreshCallback:(KeyRefreshCallback)callback;

/**
 Not available. Use initWithSubnet instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithSubnet instead.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 Start key refresh procedure, effectively refreshing keys for all nodes in subnet. All nodes that should not take part in key refresh procedure should be blacklisted.
 */
- (void)start;

/**
 Suspend ongoing key refresh procedure. Key refresh should be suspended when mobile device leaves network to avoid spurious timeouts.
 */
- (void)suspend;

/**
 Resume previously suspended key refresh procedure. Key refresh should be resumed when mobile device reenters network.
 */
- (void)resume;

@end
