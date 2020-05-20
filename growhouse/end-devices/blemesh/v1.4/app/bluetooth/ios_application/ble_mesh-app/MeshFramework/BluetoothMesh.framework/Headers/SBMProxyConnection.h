//
//  SBMProxyConnection.h
//  BluetoothMesh
//
//  Created by Łukasz Rzepka on 04.12.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMConnectableDevice.h"
#import "SBMSubnet.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Object that is used to connect and disconnect to proxy.
 */
@interface SBMProxyConnection : NSObject

/**
 Callback for successful proxy connection attempt.

 @param device Device for which the proxy connection was established.
 */
typedef void(^SBMProxyConnectionSuccess)(id<SBMConnectableDevice> device);

/**
 Callback for failed proxy connection attempt.

 @param device Device for which the proxy connection failed.
 @param error Error that specifies reason for proxy connection failure.
 */
typedef void(^SBMProxyConnectionError)(id<SBMConnectableDevice> device, NSError *error);

/**
 Callback for successful proxy connection termination attempt.
 
 @param device Device for which the proxy connection was terminated.
 */
typedef void(^SBMProxyDisconnectionSuccess)(id<SBMConnectableDevice> device);

/**
 Callback for failed proxy connection termination attempt.
 
 @param device Device for which the proxy connection termination failed.
 @param error Error that specifies reason for proxy connection termination failure.
 */
typedef void(^SBMProxyDisconnectionError)(id<SBMConnectableDevice> device, NSError *error);

/**
 Device that is used for proxy connection.
 */
@property (nonatomic, readonly) id<SBMConnectableDevice> connectableDevice;

/**
 Boolean value that determines whether proxy connection is established or not.
 */
@property (nonatomic, readonly) BOOL isConnected;

/**
 Initializes new proxy connection object.

 @param connectableDevice Device for which proxy connection object should be created.
 @return An initialized proxy connection object, or nil if an object could not be created.
 */
- (instancetype)initWithConnectableDevice:(id<SBMConnectableDevice>)connectableDevice;

/**
 Not available. Use initWithConnectableDevice instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use initWithConnectableDevice instead.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 Method used to connect to proxy.

 @param successCallback Action invoked on successful proxy connection attempt.
 @param errorCallback Action invoked when error during proxy connection attempt occurs.
 */
- (void)connectToProxy:(SBMProxyConnectionSuccess)successCallback errorCallback:(SBMProxyConnectionError)errorCallback;

/**
 Method used to terminate proxy connection.
 
 @param successCallback Action invoked on successful proxy connection termination attempt.
 @param errorCallback Action invoked when error during proxy connection termination attempt occurs.
 */
- (void)disconnect:(SBMProxyDisconnectionSuccess)successCallback errorCallback:(SBMProxyDisconnectionError)errorCallback;

/**
 The Mesh Proxy Service.
 */
+ (NSString*)meshProxyServiceUUID;

/**
 The Mesh Proxy Data In characteristic is used to write data to proxy device.
 */
+ (NSString*)characteristicMeshProxyDataInUUID;

/**
 The Mesh Proxy Data Out characteristic is used to read data from proxy device.
 */
+ (NSString*)characteristicMeshProxyDataOutUUID;

@end

NS_ASSUME_NONNULL_END
