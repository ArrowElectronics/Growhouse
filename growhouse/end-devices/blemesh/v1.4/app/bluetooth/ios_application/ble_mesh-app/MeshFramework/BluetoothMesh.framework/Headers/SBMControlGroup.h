//
//  SBMControlGroup.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 26/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMGroup.h"
#import "SBMControlValueGetSigModel.h"
#import "SBMControlValueSetSigModel.h"
#import "SBMControlValueSetVendorModel.h"
#import "SBMControlRequestParameters.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Object used to control group.
 */
@interface SBMControlGroup : NSObject

/**
 Action invoked when group status is successfully received.

 @param functionalityControl Control group object for which status was received.
 @param result Object containing result of operation.
 */
typedef void (^SBMControlGroupGetSuccess)(SBMControlGroup *functionalityControl, __kindof NSObject<SBMControlValueGetSigModel> *result);

/**
 Action invoked when group status is successfully set.

 @param functionalityControl Control group object for which status was set.
 @param request Object containing operation request.
 */
typedef void (^SBMControlGroupSetSuccess)(SBMControlGroup *functionalityControl, __kindof NSObject<SBMControlValueSetSigModel> *request);

/**
 Action invoked when group status is successfully set.

 @param functionalityControl Control group object for which status was set.
 @param request Object containing operation request.
 */
typedef void (^SBMControlGroupSetVendorSuccess)(SBMControlGroup *functionalityControl, __kindof NSObject<SBMControlValueSetVendorModel> *request);

/**
 Action invoked when group status could not be retrieved.

 @param functionalityControl Control group object for which status could not be retrieved.
 @param result Object containing result of operation.
 @param error Error describing reason of failure.
 */
typedef void (^SBMControlGroupGetError)(SBMControlGroup *functionalityControl, __kindof NSObject<SBMControlValueGetSigModel> *result, NSError *error);

/**
 Action invoked when group status could not be set.

 @param functionalityControl Control group object for which status could not be set.
 @param request Object containing operation request.
 @param error Error describing reason of failure.
 */
typedef void (^SBMControlGroupSetError)(SBMControlGroup *functionalityControl, __kindof NSObject<SBMControlValueSetSigModel> *request, NSError *error);

/**
 Action invoked when group status could not be set.
 
 @param functionalityControl Control group object for which status could not be set.
 @param request Object containing operation request.
 @param error Error describing reason of failure.
 */
typedef void (^SBMControlGroupSetVendorError)(SBMControlGroup *functionalityControl, __kindof NSObject<SBMControlValueSetVendorModel> *request, NSError *error);

/**
 Controlled group.
 */
@property (nonatomic, readonly) SBMGroup *group;

/**
 Initialize new control group object.

 @param group Group which should be controlled.
 @return An initialized control group object, or nil if an object could not be created.
 */
- (instancetype)initWithGroup:(SBMGroup * const)group;

/**
 Not available. Use initWithGroup instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithGroup instead.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 Method used to retrieve group status.

 @param getStatusClass Class of functionality which status should be retrieved.
 @param successCallback Action invoked on success.
 @param errorCallback Action invoked on failure.
 */
- (void)getStatus:(Class)getStatusClass
  successCallback:(nullable SBMControlGroupGetSuccess)successCallback
    errorCallback:(nullable SBMControlGroupGetError)errorCallback;

/**
 Method used to set group status.

 @param status Object specifying status to set.
 @param params Object specifying request parameters.
 @param successCallback Action invoked on success.
 @param errorCallback Action invoked on failure.
 */
- (void)setStatus:(__kindof NSObject<SBMControlValueSetSigModel> *)status
       parameters:(SBMControlRequestParameters *)params
  successCallback:(nullable SBMControlGroupSetSuccess)successCallback
    errorCallback:(nullable SBMControlGroupSetError)errorCallback;

/**
 Method used to set group status.

 @param status Object specifying status to set.
 @param successCallback Action invoked on success.
 @param errorCallback Action invoked on failure.
 */
- (void)setStatus:(__kindof NSObject<SBMControlValueSetVendorModel> *)status
  successCallback:(nullable SBMControlGroupSetVendorSuccess)successCallback
    errorCallback:(nullable SBMControlGroupSetVendorError)errorCallback;

@end

NS_ASSUME_NONNULL_END
