//
//  SBMLogger.h
//  ble_mesh-lib
//
//  Created by Michal Lenart on 18/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>

/**
 Log levels enum.
 */
typedef enum : NSUInteger {
    SBMLogLevelVerbose, /**< Log Level Verbose */
    SBMLogLevelDebug, /**< Log Level Debug */
    SBMLogLevelInfo, /**< Log Level Info */
    SBMLogLevelWarn, /**< Log Level Warn */
    SBMLogLevelError, /**< Log Level Error */
} SBMLogLevel;

#define SBMLogVerbose(FORMAT, ...) [SBMLogger.sharedInstance logLevel:SBMLogLevelVerbose format:FORMAT, ##__VA_ARGS__]
#define SBMLogDebug(FORMAT, ...)  [SBMLogger.sharedInstance logLevel:SBMLogLevelDebug format:FORMAT, ##__VA_ARGS__]
#define SBMLogInfo(FORMAT, ...)  [SBMLogger.sharedInstance logLevel:SBMLogLevelInfo format:FORMAT, ##__VA_ARGS__]
#define SBMLogWarn(FORMAT, ...)  [SBMLogger.sharedInstance logLevel:SBMLogLevelWarn format:FORMAT, ##__VA_ARGS__]
#define SBMLogError(FORMAT, ...)  [SBMLogger.sharedInstance logLevel:SBMLogLevelError format:FORMAT, ##__VA_ARGS__]

/**
 SBMLoggerDelegate delegate for SBMLogger.
 */
@protocol SBMLoggerDelegate

/**
 Add implementation for this method to receive logs from the framework.
 
 @param message String object. Log message received from the framework.
 @param level enum value. Level of the message.
 */
- (void)logMessage:(nullable NSString*)message withLevel:(SBMLogLevel)level;

@end

/**
 Bluetooth Mesh logger. Setup this object to receive logs from the framework.
 */
@interface SBMLogger : NSObject

/**
 SBMLogger instance.
 */
+ (nonnull instancetype)sharedInstance;

/**
 Not available. Use sharedInstance instead.
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 Not available. Use sharedInstance instead.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 SBMLogger delegate. Set this property to receive logs.
 */
@property (nonatomic, nullable, weak) id<SBMLoggerDelegate> delegate;

/**
 Setup logs for framework.
 
 @param level enum value. Level of supported logs.
 @param format String object. Log format.
 */
- (void)logLevel:(SBMLogLevel)level format:(nullable NSString*) format, ... NS_FORMAT_FUNCTION(2, 3);

@end
