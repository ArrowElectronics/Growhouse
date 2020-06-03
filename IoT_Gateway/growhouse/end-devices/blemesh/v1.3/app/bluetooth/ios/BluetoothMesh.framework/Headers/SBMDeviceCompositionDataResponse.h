//
//  SBMDeviceCompositionDataResponse.h
//  ble_mesh-lib
//
//  Created by Lukasz Rzepka on 04.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>

@interface SBMDeviceCompositionDataResponse: NSObject
@property (nonatomic, strong) NSData* deviceUUIDData;
@property (nonatomic, strong) NSData* dcd;
@property (nonatomic) NSInteger cancellationHandle;
@end
