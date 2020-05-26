//
//  SBMConfigOperation.h
//  ble_mesh-lib
//
//  Created by Lukasz Rzepka on 02.05.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SBMMeshNetwork.h"
#import "SBMMeshNode.h"
#import "SBMMeshGroup.h"
#import "SBMConfigOperationTypeEnum.h"
#include <mesh_foundation_defs.h>

typedef NS_ENUM(NSUInteger, SBMConfigStepType) {
    SBMConfigStepTypeDone,
    SBMConfigStepTypeClearPub,
    SBMConfigStepTypeClearSub,
    SBMConfigStepTypeGetBindings,
    SBMConfigStepTypeUnbind,
    SBMConfigStepTypeRemoveOldKey,
    SBMConfigStepTypeAddKey,
    SBMConfigStepTypeSetBind,
    SBMConfigStepTypePub,
    SBMConfigStepTypeSub,
    SBMConfigStepTypeRemoveSub,
    SBMConfigStepTypeProxy,
    SBMConfigStepTypeEnableProxy,
    SBMConfigStepTypeDisableProxy,
    SBMConfigStepTypeResetDevice,
    SBMConfigStepTypeRelay,
    SBMConfigStepTypEenableRelay,
    SBMConfigStepTypeDisableRelay,
    SBMConfigStepTypeFriend,
    SBMConfigStepTypeEnableFriend,
    SBMConfigStepTypeDisableFriend,
    SBMConfigStepTypeDcd
};

@interface SBMConfigOperation : NSObject
- (instancetype)initWithType:(SBMConfigOperationType)type node:(SBMMeshNode*)node group:(SBMMeshGroup*)group;
@property (nonatomic, strong) SBMMeshNode* node;
@property (nonatomic, strong) SBMMeshGroup* group;
@property (nonatomic, strong) SBMModel* model;
@property (nonatomic, strong) SBMElement* element;
@property (nonatomic) SBMConfigOperationType type;
@property (nonatomic) mesh_foundation_status_t status;
@property (nonatomic) mesh_foundation_result_t* result;
@property (nonatomic, strong) NSArray<NSNumber*>* steps;
@property (nonatomic) NSUInteger currentStep;
@property (nonatomic) NSUInteger attempt;
@end
