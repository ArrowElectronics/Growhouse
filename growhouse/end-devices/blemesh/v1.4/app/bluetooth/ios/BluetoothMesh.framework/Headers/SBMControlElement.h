//
//  SBMControlElement.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 25/10/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMElement.h"

#import "SBMControlValueGetSigModel.h"
#import "SBMControlValueSetSigModel.h"
#import "SBMControlValueSetVendorModel.h"
#import "SBMControlRequestParameters.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Object used to get and set element states.
 */
@interface SBMControlElement : NSObject

/**
 Action invoked when element status is successfully received.

 @param controlElement Control element object for which status was received.
 @param result Object containing result of operation.
 */
typedef void (^SBMControlElementGetSuccess)(SBMControlElement *controlElement, __kindof NSObject<SBMControlValueGetSigModel> *result);

/**
 Action invoked when element status is successfully set.

 @param controlElement Control element object for which status was set.
 @param request Object containing operation request.
 */
typedef void (^SBMControlElementSetSuccess)(SBMControlElement *controlElement, __kindof NSObject<SBMControlValueSetSigModel> *request);

/**
 Action invoked when element status is successfully set.

 @param controlElement Control element object for which status was set.
 @param request Object containing operation request.
 */
typedef void (^SBMControlElementSetVendorSuccess)(SBMControlElement *controlElement, __kindof NSObject<SBMControlValueSetVendorModel> *request);

/**
 Action invoked when element status could not be retrieved.

 @param controlElement Control element object for which status could not be retrieved.
 @param result Object containing result of operation.
 @param error Error describing reason of failure.
 */
typedef void (^SBMControlElementGetError)(SBMControlElement *controlElement, __kindof NSObject<SBMControlValueGetSigModel> *result, NSError *error);

/**
 Action invoked when element status could not be set.

 @param controlElement Control element object for which status could not be set.
 @param request Object containing operation request.
 @param error Error describing reason of failure.
 */
typedef void (^SBMControlElementSetError)(SBMControlElement *controlElement, __kindof NSObject<SBMControlValueSetSigModel> *request, NSError *error);

/**
 Action invoked when element status could not be set.

 @param controlElement Control element object for which status could not be set.
 @param request Object containing operation request.
 @param error Error describing reason of failure.
 */
typedef void (^SBMControlElementSetVendorError)(SBMControlElement *controlElement, __kindof NSObject<SBMControlValueSetVendorModel> *request, NSError *error);

/**
 Controlled element.
 */
@property (nonatomic, readonly) SBMElement *element;

/**
 Group used to control element.
 */
@property (nonatomic, readonly) SBMGroup *group;

/**
 Initialize new control element object.

 @param element Element which should be controlled.
 @param group Element's group.
 @return An initialized control element object, or nil if an object could not be created.
 */
- (instancetype)initWithElement:(SBMElement *)element inGroup:(SBMGroup *)group;

/**
 Not available. Use initWithElement instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithElement instead.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 Method used to retrieve SIG model/functionality status.
 
 For example getStatusClass can be set as SBMLightningCTLTemperature.class in ObjC
 or SBMLightningCTLTemperature.self in Swift.
 getStatusClass must immplement SBMControlValueGetSigModel.

 @param getStatusClass Class of functionality which status should be retrieved.
 @param successCallback Action invoked on success.
 @param errorCallback Action invoked on failure.
 */
- (void)getStatus:(Class)getStatusClass
  successCallback:(nullable SBMControlElementGetSuccess)successCallback
    errorCallback:(nullable SBMControlElementGetError)errorCallback;

/**
 Method used to set SIG model/functionality status.
 
 For example use SBMLightningCTLSet object.

 @param status Object specifying status to set.
 @param params Object specifying request parameters.
 @param successCallback Action invoked on success.
 @param errorCallback Action invoked on failure.
 */
- (void)setStatus:(__kindof NSObject<SBMControlValueSetSigModel> *)status
       parameters:(SBMControlRequestParameters *)params
  successCallback:(nullable SBMControlElementSetSuccess)successCallback
    errorCallback:(nullable SBMControlElementSetError)errorCallback;

/**
 Method used to set Vendor model/functionality status.
 
 It is needed to provide implementation by application developer
 for SBMControlValueSetVendorModel to use this functionality.

 @param status Object specifying status to set.
 @param successCallback Action invoked on success.
 @param errorCallback Action invoked on failure.
 */
- (void)setStatus:(__kindof NSObject<SBMControlValueSetVendorModel> *)status
  successCallback:(nullable SBMControlElementSetVendorSuccess)successCallback
    errorCallback:(nullable SBMControlElementSetVendorError)errorCallback;

/**
 Method used to subscribe to SIG model status.
 To subscribe to vendor model status use SBMVendorModelNotifications.

 @param subStatusClass Class of functionality which status should be subscribed to.
 @param callback Action invoked on status change.
 @return Boolean value that determines whether subscription was successfully added.
 */
- (BOOL)subscribeStatus:(Class)subStatusClass callback:(SBMControlElementGetSuccess)callback;

/**
 Method used to unsubscribe from SIG model status.
 To unsubscribe from vendor model status use SBMVendorModelNotifications.

 @param unsubStatusClass Class of functionality which status should be unsubscribed from.
 @return Boolean value that determines whether subscription was successfully removed.
 */
- (BOOL)unsubscribeStatus:(Class)unsubStatusClass;

@end

NS_ASSUME_NONNULL_END
