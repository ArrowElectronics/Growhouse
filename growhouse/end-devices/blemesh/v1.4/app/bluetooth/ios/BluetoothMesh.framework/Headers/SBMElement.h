//
//  SBMElement.h
//  ble_mesh-lib
//
//  Created by Lukasz Rzepka on 04.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMSigModel.h"
#import "SBMVendorModel.h"
@class SBMNode;

NS_ASSUME_NONNULL_BEGIN

/**
 Each node contains one or more Elements. Element is a container for a Models contained by Node.
 */
@interface SBMElement : NSObject

/**
 Unicast Address.
 */
@property (nonatomic, readonly) NSInteger address;
 
/**
 Array of SIG Models contained by Element.
 */
@property (nonatomic, readonly) NSArray<SBMSigModel *> *sigModels;


/**
 Array of Vendor Models contained by Element.
 */
@property (nonatomic, readonly) NSArray<SBMVendorModel *> *vendorModels;

/**
 Element's node.
 */
@property (nonatomic, readonly, weak) SBMNode* node;

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
