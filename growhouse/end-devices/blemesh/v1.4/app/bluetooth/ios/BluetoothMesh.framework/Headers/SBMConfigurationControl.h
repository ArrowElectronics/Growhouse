//
//  SBMConfigurationControl.h
//  BluetoothMesh
//
//  Created by Łukasz Rzepka on 07.11.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMNode.h"
#import "SBMSubnet.h"
#import "SBMDeviceCompositionData.h"

NS_ASSUME_NONNULL_BEGIN

#pragma SBMConfigurationControl

/**
 SBMConfigurationControl methods are for configuring a node over the mesh network by sending configuration model messages. Note that the API is asynchronous; unless there is an immediate local failure.
 Important: It is not needed to keep object by application. Framework will do it.
*/

@interface SBMConfigurationControl : NSObject

#pragma Get callbacks

/**
 @param node SBMNode node to which the message was sent.
 @param enabled BOOL parameter for whether proxy is enabled or not.
 */
typedef void(^SBMCheckNodeProxySuccess)(SBMNode* node, BOOL enabled);

/**
 @param node SBMNode to which the message was sent.
 @param error NSError error received during check proxy state.
 */
typedef void(^SBMCheckNodeProxyError)(SBMNode* node, NSError* error);

/**
 @param node SBMNode node to which the message was sent.
 @param enabled BOOL parameter for whether relay is enabled or not.
 @param retransmissionCount UInt8 parameter for relay retransmission count.
 @param retransmissionInterval UInt8 parameter for relay retransmission interval.
 */
typedef void(^SBMCheckNodeRelaySuccess)(SBMNode* node, BOOL enabled, UInt8 retransmissionCount, UInt8 retransmissionInterval);

/**
 @param node SBMNode node to which the message was sent.
 @param error NSError error received during check relay state.
 */
typedef void(^SBMCheckNodeRelayError)(SBMNode* node, NSError* error);

/**
 @param node SBMNode node to which the message was sent.
 @param enabled BOOL parameter for whether friend is enabled or not.
 */
typedef void(^SBMCheckNodeFriendSuccess)(SBMNode* node, BOOL enabled);

/**
 @param node SBMNode node to which the message was sent.
 @param error NSError error received during check friend state.
 */
typedef void(^SBMCheckNodeFriendError)(SBMNode* node, NSError* error);

/**
 @param node SBMNode node to which the message was sent.
 @param enabled BOOL parameter for whether node identity is enabled or not.
 @param subnet SBMSubnet checked subnet. Node identity can be set only for one subnet.
 */
typedef void(^SBMCheckNodeIdentitySuccess)(SBMNode* node, BOOL enabled, SBMSubnet* subnet);

/**
 @param node SBMNode node to which the message was sent.
 @param error NSError error received during check node identity state.
 */
typedef void(^SBMCheckNodeIdentityError)(SBMNode* node, NSError* error);

/**
 @param node SBMNode node to which the message was sent.
 @param publicationAddress Heartbeat publication address.
 @param publishCountLogarithm Heartbeat publish count 2's logarithm value.
 @param publishPeriodLogarithm Heartbeat publish period 2's logarithm value.
 @param timeToLive Heartbeat time to live.
 @param features Feature bitmask for heartbeat triggering.
 @param publishCryptoSubnetKeyIndex Network key index of the key used to encrypt the published messages.
 */
typedef void(^SBMCheckNodeHeartbeatPublicationSuccess)(SBMNode* node, UInt16 publicationAddress, UInt8 publishCountLogarithm, UInt8 publishPeriodLogarithm, UInt8 timeToLive, UInt16 features, UInt16 publishCryptoSubnetKeyIndex);

/**
 @param node SBMNode node to which the message was sent.
 @param error NSError error received during check heartbeat publication.
 */
typedef void(^SBMCheckNodeHeartbeatPublicationError)(SBMNode* node, NSError* error);

/**
 @param node SBMNode node to which the message was sent.
 @param sourceAddress Heartbeat source address.
 @param destinationAddress Heartbeat destination address.
 @param subscribeCountLogarithm Heartbeat subscribe count 2's logarithm value.
 @param subscribePeriodLogarithm Heartbeat subscribe period 2's logarithm value.
 @param hopMinimum Minimum hops value registered when receiving Heartbeat messages.
 @param hopMaximum Maximum hops value registered when receiving Heartbeat messages.
 */
typedef void(^SBMCheckNodeHeartbeatSubscriptionSuccess)(SBMNode* node, UInt16 sourceAddress, UInt16 destinationAddress, UInt8 subscribeCountLogarithm, UInt8 subscribePeriodLogarithm, UInt8 hopMinimum, UInt8 hopMaximum);

/**
 @param node SBMNode node to which the message was sent.
 @param error NSError error received during check heartbeat subscription.
 */
typedef void(^SBMCheckNodeHeartbeatSubscriptionError)(SBMNode* node, NSError* error);

/**
 @param node SBMNode node to which the message was sent.
 @param retransmissionCount Network retransmission count.
 @param retransmissionInterval Network retransmission interval.
 */
typedef void(^SBMCheckNodeRetransmissionConfigurationSuccess)(SBMNode* node, UInt8 retransmissionCount, UInt8 retransmissionInterval);

/**
 @param node SBMNode node to which the message was sent.
 @param error NSError error received during check retransmission configuration.
 */
typedef void(^SBMCheckNodeRetransmissionConfigurationError)(SBMNode* node, NSError* error);

/**
 @param configurationControl SBMConfigurationControl reference to configuration control.
 @param dcd SBMDeviceCompositionData device composition data of node.
 @param elements NSArray parsed Elements from dcd.
 */
typedef void(^SBMGetDeviceCompositionDataSuccess)(SBMConfigurationControl *configurationControl, SBMDeviceCompositionData* dcd, NSArray<SBMElement*>* elements);

/**
 @param configurationControl SBMConfigurationControl reference to configuration control.
 @param error NSError received error.
 */
typedef void(^SBMGetDeviceCompositionDataError)(SBMConfigurationControl *configurationControl, NSError* error);

#pragma Set callbacks

/**
 @param node SBMNode node to which the message was sent.
 @param enabled BOOL parameter for changed proxy state.
 */
typedef void(^SBMSetNodeProxySuccess)(SBMNode* node, BOOL enabled);

/**
 @param node SBMNode node to which the message was sent.
 @param error NSError error received during change proxy state.
 */
typedef void(^SBMSetNodeProxyError)(SBMNode* node, NSError* error);

/**
 @param node SBMNode node to which the message was sent.
 @param enabled BOOL parameter for changed proxy state.
 @param retransmissionCount UInt8 parameter for changed relay retransmission count.
 @param retransmissionInterval UInt8 parameter for changed relay retransmission interval.
 */
typedef void(^SBMSetNodeRelaySuccess)(SBMNode* node, BOOL enabled, UInt8 retransmissionCount, UInt8 retransmissionInterval);

/**
 @param node SBMNode node to which the message was sent.
 @param error NSError error received during change relay state.
 */
typedef void(^SBMSetNodeRelayError)(SBMNode* node, NSError* error);

/**
 @param node SBMNode node to which the message was sent.
 @param enabled BOOL parameter for changed friend state.
 */
typedef void(^SBMSetNodeFriendSuccess)(SBMNode* node, BOOL enabled);

/**
 @param node SBMNode node to which the message was sent.
 @param error NSError error received during change friend state.
 */
typedef void(^SBMSetNodeFriendError)(SBMNode* node, NSError* error);

/**
 @param node SBMNode node to which the message was sent.
 @param enabled BOOL parameter for changed node identity state.
 @param subnet SBMSubnet subnet for which the node identity has been changed.
 */
typedef void(^SBMSetNodeIdentitySuccess)(SBMNode* node, BOOL enabled, SBMSubnet* subnet);

/**
 @param node SBMNode node to which the message was sent.
 @param error NSError error received during change node identity state.
 */
typedef void(^SBMSetNodeIdentityError)(SBMNode* node, NSError* error);

/**
 @param node SBMNode node to which the message was sent.
 @param publicationAddress Heartbeat publication address.
 @param publishCountLogarithm Heartbeat publish count 2's logarithm value.
 @param publishPeriodLogarithm Heartbeat publish period 2's logarithm value.
 @param timeToLive Heartbeat time to live.
 @param features Feature bitmask for heartbeat triggering.
 @param publishCryptoSubnetKeyIndex Network key index of the key used to encrypt the published messages.
 */
typedef void(^SBMSetNodeHeartbeatPublicationSuccess)(SBMNode* node, UInt16 publicationAddress, UInt8 publishCountLogarithm, UInt8 publishPeriodLogarithm, UInt8 timeToLive, UInt16 features, UInt16 publishCryptoSubnetKeyIndex);

/**
 @param node SBMNode node to which the message was sent.
 @param error NSError error received during set heartbeat publication.
 */
typedef void(^SBMSetNodeHeartbeatPublicationError)(SBMNode* node, NSError* error);

/**
 @param node SBMNode node to which the message was sent.
 @param sourceAddress Heartbeat source address.
 @param destinationAddress Heartbeat destination address.
 @param subscribeCountLogarithm Heartbeat subscribe count 2's logarithm value.
 @param subscribePeriodLogarithm Heartbeat subscribe period 2's logarithm value.
 @param hopMinimum Minimum hops value registered when receiving Heartbeat messages.
 @param hopMaximum Maximum hops value registered when receiving Heartbeat messages.
 */
typedef void(^SBMSetNodeHeartbeatSubscriptionSuccess)(SBMNode* node, UInt16 sourceAddress, UInt16 destinationAddress, UInt8 subscribeCountLogarithm, UInt8 subscribePeriodLogarithm, UInt8 hopMinimum, UInt8 hopMaximum);

/**
 @param node SBMNode node to which the message was sent.
 @param error NSError error received during set heartbeat subscription.
 */
typedef void(^SBMSetNodeHeartbeatSubscriptionError)(SBMNode* node, NSError* error);

/**
 @param node SBMNode node to which the message was sent.
 @param retransmissionCount Network retransmission count.
 @param retransmissionInterval Network retransmission interval.
 */
typedef void(^SBMSetNodeRetransmissionConfigurationSuccess)(SBMNode* node, UInt8 retransmissionCount, UInt8 retransmissionInterval);

/**
 @param node SBMNode node to which the message was sent.
 @param error NSError error received during set retransmission configuration.
 */
typedef void(^SBMSetNodeRetransmissionConfigurationError)(SBMNode* node, NSError* error);

/**
 @param node SBMNode node to which the message was sent.
 */
typedef void(^SBMFactoryResetSuccess)(SBMNode* node);

/**
 @param node SBMNode node to which the message was sent.
 @param error NSError error received during factory reset of node.
 */
typedef void(^SBMFactoryResetError)(SBMNode* node, NSError* error);

/**
 Node to configure.
*/
@property (nonatomic, readonly) SBMNode* node;

/**
 SBMConfigurationControl initialization.
 @param node SBMNode object to configure.
*/
- (instancetype)initWithNode:(SBMNode*)node;

/**
 Not available. Use initWithNode instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithNode instead.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 Check proxy state.
*/
- (void)checkProxyStatus:(SBMCheckNodeProxySuccess)successCallback
           errorCallback:(SBMCheckNodeProxyError)errorCallback;

/**
 Check relay state.
 */
- (void)checkRelayStatus:(SBMCheckNodeRelaySuccess)successCallback
           errorCallback:(SBMCheckNodeRelayError)errorCallback;

/**
 Check friend state.
 */
- (void)checkFriendStatus:(SBMCheckNodeFriendSuccess)successCallback
            errorCallback:(SBMCheckNodeFriendError)errorCallback;

/**
 Check node identity state for subnet.
 */
- (void)checkNodeIdentityStatus:(SBMSubnet*)subnet
                successCallback:(SBMCheckNodeIdentitySuccess)successCallback
                  errorCallback:(SBMCheckNodeIdentityError)errorCallback;

/**
 Gets a node's heartbeat publish parameters.

 @param successCallback Action invoked on successfull heartbeat publication check operation.
 @param errorCallback Action invoked on failed heartbeat publication check operation.
 */
- (void)checkHeartbeatPublicationStatus:(SBMCheckNodeHeartbeatPublicationSuccess)successCallback
                          errorCallback:(SBMCheckNodeHeartbeatPublicationError)errorCallback;

/**
 Gets a node's heartbeat subscription parameters.

 @param successCallback Action invoked on successfull heartbeat subscription check operation.
 @param errorCallback Action invoked on failed heartbeat subscription check operation.
 */
- (void)checkHeartbeatSubscriptionStatus:(SBMCheckNodeHeartbeatSubscriptionSuccess)successCallback
                           errorCallback:(SBMCheckNodeHeartbeatSubscriptionError)errorCallback;

/**
 Gets a node's retransmission configuration.
 
 @param successCallback Action invoked on successfull retransmission configuration check operation.
 @param errorCallback Action invoked on failed retransmission configuration check operation.
 */
- (void)checkRetransmissionConfigurationStatus:(SBMCheckNodeRetransmissionConfigurationSuccess)successCallback
                                 errorCallback:(SBMCheckNodeRetransmissionConfigurationError)errorCallback;

/**
 Set proxy state.
 */
- (void)setProxy:(BOOL)enabled
            with:(SBMSetNodeProxySuccess)successCallback
   errorCallback:(SBMSetNodeProxyError)errorCallback;

/**
 Set relay state.
 */
- (void)setRelay:(BOOL)enabled
     retransmissionCount:(UInt8)retransmissionCount
  retransmissionInterval:(UInt8)retransmissionInterval
                    with:(SBMSetNodeRelaySuccess)successCallback
           errorCallback:(SBMSetNodeRelayError)errorCallback;

/**
 Set friend state.
 */
- (void)setFriend:(BOOL)enabled
             with:(SBMSetNodeFriendSuccess)successCallback
    errorCallback:(SBMSetNodeFriendError)errorCallback;

/**
 Set node identity state for subnet.
 */
- (void)setNodeIdentity:(SBMSubnet*)subnet
                enabled:(BOOL)enabled
                   with:(SBMSetNodeIdentitySuccess)successCallback
          errorCallback:(SBMSetNodeIdentityError)errorCallback;

/**
 Sets a node's heartbeat publish parameters.

 @param publicationAddress Heartbeat publication address.
 @param publishCountLogarithm Heartbeat publish count 2's logarithm value.
 @param publishPeriodLogarithm Heartbeat publish period 2's logarithm value.
 @param timeToLive Heartbeat time to live.
 @param features Feature bitmask for heartbeat triggering.
 @param publishCryptoSubnetKeyIndex Network key index of the key used to encrypt the published messages.
 @param successCallback Action invoked on successfull heartbeat publication set operation.
 @param errorCallback Action invoked on failed heartbeat publication set operation.
 */
- (void)setHeartbeatPublication:(UInt16)publicationAddress
          publishCountLogarithm:(UInt8)publishCountLogarithm
         publishPeriodLogarithm:(UInt8)publishPeriodLogarithm
                     timeToLive:(UInt8)timeToLive
                       features:(UInt16)features
    publishCryptoSubnetKeyIndex:(UInt16)publishCryptoSubnetKeyIndex
                           with:(SBMSetNodeHeartbeatPublicationSuccess)successCallback
                  errorCallback:(SBMSetNodeHeartbeatPublicationError)errorCallback;

/**
 Sets a node's heartbeat subscription parameters.

 @param sourceAddress Heartbeat source address.
 @param destinationAddress Heartbeat destination address.
 @param period Heartbeat subscription period.
 @param successCallback Action invoked on successfull heartbeat subscription set operation.
 @param errorCallback Action invoked on failed heartbeat subscription set operation.
 */
- (void)setHeartbeatSubscription:(UInt16)sourceAddress
              destinationAddress:(UInt16)destinationAddress
                          period:(UInt16)period
                            with:(SBMSetNodeHeartbeatSubscriptionSuccess)successCallback
                   errorCallback:(SBMSetNodeHeartbeatSubscriptionError)errorCallback;

/**
 Sets a node's retransmission configuration.
 
 @param retransmissionCount Network retransmission count. Use 0 to turn retransmissions off.
 @param retransmissionInterval Network retransmission interval.
 @param successCallback Action invoked on successfull retransmission configuration set operation.
 @param errorCallback Action invoked on failed retransmission configuration set operation.
 */
- (void)setRetransmissionConfiguration:(UInt8)retransmissionCount
                retransmissionInterval:(UInt8)retransmissionInterval
                                  with:(SBMSetNodeRetransmissionConfigurationSuccess)successCallback
                         errorCallback:(SBMSetNodeRetransmissionConfigurationError)errorCallback;

/**
 Send factory reset node message.
 */
- (void)factoryReset:(SBMFactoryResetSuccess)successCallback
       errorCallback:(SBMFactoryResetError)errorCallback;

/**
 Get device composition data state.
 */
- (void)getDeviceCompositionData:(SBMGetDeviceCompositionDataSuccess)successCallback
                   errorCallback:(SBMGetDeviceCompositionDataError)errorCallback;

@end

NS_ASSUME_NONNULL_END
