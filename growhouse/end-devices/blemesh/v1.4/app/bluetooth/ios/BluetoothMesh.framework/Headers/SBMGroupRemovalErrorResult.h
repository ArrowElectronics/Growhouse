//
//  SBMGroupRemovalErrorResult.h
//  BluetoothMesh
//
//  Created by Łukasz Rzepka on 08.12.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMTuple.h"
#import "SBMNode.h"
@class SBMGroup;

NS_ASSUME_NONNULL_BEGIN

/**
 Object that contains information about result of group removal operation.
 */
@interface SBMGroupRemovalErrorResult : NSObject

/**
 Node that was successfully unbound during group removal.
 */
typedef SBMNode *SuccessResultOfUnbindNode;

/**
 SIG Model that was successfully unbound during group removal.
 */
typedef SBMSigModel *SuccessResultOfUnbindSigModel;

/**
 Vendor Model that was successfully unbound during group removal.
 */
typedef SBMVendorModel *SuccessResultOfUnbindVendorModel;

/**
 Node that was not unbound during group removal, and corresponding error.
 */
typedef SBMTuple<SBMNode *, NSError *> *ErrorResultOfUnbindNode;

/**
 SIG Model that was not unbound during group removal, and corresponding error.
 */
typedef SBMTuple<SBMSigModel *, NSError *> *ErrorResultOfUnbindSigModel;

/**
 Vendor Model that was not unbound during group removal, and corresponding error.
 */
typedef SBMTuple<SBMVendorModel *, NSError *> *ErrorResultOfUnbindVendorModel;

/**
 Array of Nodes that were successfully unbound during group removal.
 */
@property (nonatomic, strong) NSArray<SuccessResultOfUnbindNode> *unbindedNodes;

/**
 Array of SIG Models that were successfully unbound during group removal.
 */
@property (nonatomic, strong) NSArray<SuccessResultOfUnbindSigModel> *unbindedSigModels;

/**
 Array of Vendor Models that were successfully unbound during group removal.
 */
@property (nonatomic, strong) NSArray<SuccessResultOfUnbindVendorModel> *unbindedVendorModels;

/**
 Array of Nodes that were not unbound during group removal, and corresponding errors.
 */
@property (nonatomic, strong) NSArray<ErrorResultOfUnbindNode> *notUnbindedNodes;

/**
 Array of SIG Models that were not unbound during group removal, and corresponding errors.
 */
@property (nonatomic, strong) NSArray<ErrorResultOfUnbindSigModel> *notUnbindedSigModels;

/**
 Array of Vendor Models that were not unbound during group removal, and corresponding errors.
 */
@property (nonatomic, strong) NSArray<ErrorResultOfUnbindVendorModel> *notUnbindedVendorModels;

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

