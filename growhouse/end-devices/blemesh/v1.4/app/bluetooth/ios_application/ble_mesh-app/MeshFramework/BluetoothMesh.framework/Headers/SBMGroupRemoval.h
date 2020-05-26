//
//  SBMGroupRemoval.h
//  BluetoothMesh
//
//  Created by Łukasz Rzepka on 08.12.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMGroupRemovalErrorResult.h"
@class SBMGroup;

/**
 Action invoked on successfull group removal.

 @param group Group that was removed.
 */
typedef void(^SBMGroupRemovalSuccess)(SBMGroup *group);

/**
 Action invoked on failed group removal.

 @param group Group that was being removed.
 @param result Result of group removal operation.
 @param error Error describing reason of failure.
 */
typedef void(^SBMGroupRemovalError)(SBMGroup *group, SBMGroupRemovalErrorResult *result, NSError *error);

/**
 Action invoked on failed group force removal.

 @param group Group that was being removed.
 @param error Error describing reason of failure.
 */
typedef void(^SBMGroupForceRemovalError)(SBMGroup* group, NSError* error);
