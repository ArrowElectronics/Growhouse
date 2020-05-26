//
//  SBMNetworkRemovalErrorResult.h
//  BluetoothMesh
//
//  Created by Łukasz Rzepka on 05.12.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMTuple.h"
#import "SBMNode.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Object that contains information about result of subnet removal operation.
 */
@interface SBMSubnetRemovalErrorResult : NSObject

/**
 Node that was successfully removed during subnet removal.
 */
typedef SBMNode *SuccessResultOfRemovalNode;

/**
 Node that was not removed during subnet removal, and corresponding error.
 */
typedef SBMTuple<SBMNode *, NSError *> *ErrorResultOfRemovalNode;

/**
 Array of nodes that were successfully removed during subnet removal.
 */
@property (nonatomic, strong) NSArray<SuccessResultOfRemovalNode> *removedNodes;

/**
 Array of nodes that were not removed during subnet removal, and corresponding errors.
 */
@property (nonatomic, strong) NSArray<ErrorResultOfRemovalNode> *notRemovedNodes;

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
