//
//  SBMFunctionalityBinder.h
//  BluetoothMesh
//
//  Created by Arkadiusz Marszałek on 06/11/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMGroup.h"
#import "SBMModel.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Action invoked when model binding operation has succeeded.

 @param succeededModels List of Model objects that were successfully bound to group.
 @param group Group object that models were bound to.
 */
typedef void(^ModelBindingSuccessCallback)(NSArray<NSObject<SBMModel>*>* succeededModels, SBMGroup* group);

/**
 Action invoked when model binding operation has failed.

 @param succeededModels List of Model objects that were successfully bound to group.
 @param group Group object that models were bound to.
 @param error Error describing reason of failure.
 */
typedef void(^ModelBindingErrorCallback)(NSArray<NSObject<SBMModel>*>* succeededModels, SBMGroup* group, NSError* _Nullable error);

/**
 Action invoked when model unbinding operation has succeeded.

 @param succeededModels List of Model objects that were successfully unbound from group.
 @param group Group object that models were unbound from.
 */
typedef void(^ModelUnbindingSuccessCallback)(NSArray<NSObject<SBMModel>*>* succeededModels, SBMGroup* group);

/**
 Action invoked when model unbinding operation has failed.

 @param succeededModels List of Model objects that were successfully unbound from group.
 @param group Group object that models were unbound from.
 @param error Error describing reason of failure.
 */
typedef void(^ModelUnbindingErrorCallback)(NSArray<NSObject<SBMModel>*>* succeededModels, SBMGroup* group, NSError* _Nullable error);

/**
 Functionality Binder object. It is used to bind models with group.
 */
@interface SBMFunctionalityBinder : NSObject

/**
 Initialize new Functionality Binder object.

 @param group Group to which models will be bound.
 @return An initialized Functionality Binder object, or nil if an object could not be created.
 */
- (instancetype)initWithGroup:(SBMGroup*)group;

/**
 Not available. Use initWithGroup instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithGroup instead.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 Binds model to group.

 @param model Model object which should be bound with group.
 @param successCallback Action invoked on successfull binding operation.
 @param errorCallback Action invoked on failed binding operation.
 */
- (void)bindModel:(NSObject<SBMModel>*)model successCallback:(ModelBindingSuccessCallback)successCallback errorCallback:(ModelBindingErrorCallback)errorCallback;

/**
 Binds array of models to group.

 @param models Array of Model objects which should be bound with group.
 @param successCallback Action invoked on successfull binding operation.
 @param errorCallback Action invoked on failed binding operation.
 */
- (void)bindModels:(NSArray<NSObject<SBMModel>*>*)models successCallback:(ModelBindingSuccessCallback)successCallback errorCallback:(ModelBindingErrorCallback)errorCallback;

/**
 Unbinds model from group.

 @param model Model object which should be unbound from group.
 @param successCallback Action invoked on successfull unbinding operation.
 @param errorCallback Action invoked on failed unbinding operation.
 */
- (void)unbindModel:(NSObject<SBMModel>*)model successCallback:(ModelUnbindingSuccessCallback)successCallback errorCallback:(ModelUnbindingErrorCallback)errorCallback;

/**
 Unbind array of models from group.

 @param models Array of Model objects which should be unbound from group.
 @param successCallback Action invoked on successfull unbinding operation.
 @param errorCallback Action invoked on failed unbinding operation.
 */
- (void)unbindModels:(NSArray<NSObject<SBMModel>*>*)models successCallback:(ModelUnbindingSuccessCallback)successCallback errorCallback:(ModelUnbindingErrorCallback)errorCallback;

@end

NS_ASSUME_NONNULL_END
