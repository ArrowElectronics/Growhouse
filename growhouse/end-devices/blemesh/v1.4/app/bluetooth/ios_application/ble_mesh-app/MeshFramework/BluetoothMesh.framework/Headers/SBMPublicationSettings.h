//
//  SBMPublicationSettings.h
//  BluetoothMesh
//
//  Created by Piotr Sarna on 21/11/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMNotificationSettings.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Object that represents publication settings.
 */
@interface SBMPublicationSettings : SBMNotificationSettings

/**
 An integer value that determines publication period.
 */
@property (nonatomic) NSInteger period;

/**
 A boolean value that determines whether model should publish using friendship credentials flag on or off.
 */
@property (nonatomic) BOOL isFriendshipCredential;

/**
 An integer value that determines publication time to live.
 */
@property (nonatomic) NSInteger ttl;

/**
 An integer value that determines retransmission count.
 */
@property (nonatomic) NSInteger retransmitCount;

/**
 An integer value that determines retransmission interval.
 */
@property (nonatomic) NSInteger retransmitIntervalSteps;

@end

NS_ASSUME_NONNULL_END
