//
//  SBMTuple.h
//  ble_mesh-lib
//
//  Created by Lukasz Rzepka on 06.06.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>

/**
 Tuple with two elements.
 */
@interface SBMTuple<T1, T2> : NSObject <NSCopying>

/**
 Tuple's first element.
 */
@property (nonatomic, strong) T1 first;

/**
 Tuple's second element.
 */
@property (nonatomic, strong) T2 second;

/**
 Initialize new tuple.

 @param first First element.
 @param second Second element.
 @return An initialized tuple, or nil if an object could not be created.
 */
+ (SBMTuple*)tupleWithFirst:(T1)first second:(T2)second;

/**
 Check whether tuples are equal or not.

 @param tuple Tuple to check.
 @return Boolean value indicating whether tuples are equal.
 */
- (BOOL)isEqualToTuple:(SBMTuple*)tuple;

@end
