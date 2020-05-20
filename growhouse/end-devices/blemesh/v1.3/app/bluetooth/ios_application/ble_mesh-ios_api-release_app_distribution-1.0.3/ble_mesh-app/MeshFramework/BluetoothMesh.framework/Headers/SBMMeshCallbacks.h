//
//  SBMMeshCallbacks.h
//  ble_mesh-lib
//
//  Created by Lukasz Rzepka on 10.04.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import "SBMConfigOperation.h"
#import "SBMDeviceCompositionDataResponse.h"

typedef void(^ProxyConnectCallback)(NSInteger gattHandle, BOOL success);
typedef void(^SBMConfigureNodeCallback)(SBMConfigOperation* operation);

