//
// command-context.h
//
// May 20, 2015
//
// Declarations of command context structs.
//
// Copyright 2015 Silicon Laboratories, Inc.
//

#ifndef SILABS_COMMAND_CONTEXT_H
#define SILABS_COMMAND_CONTEXT_H

#include "app/util/ezsp/ezsp-enum.h"

typedef struct {
  bool consumed;
  EzspStatus status;

  union {
    EzspValueId valueId;
    EzspExtendedValueId extendedValueId;
  };

  uint32_t characteristics;

  uint8_t valueLength;
  uint8_t value[128];
} EmberAfPluginEzspValueCommandContext;

typedef struct {
  bool consumed;

  EzspPolicyId policyId;
  EzspStatus status;
  EzspDecisionId decisionId;
} EmberAfPluginEzspPolicyCommandContext;

typedef struct {
  bool consumed;
  EzspConfigId configId;
  EzspStatus status;
  uint16_t value;
} EmberAfPluginEzspConfigurationValueCommandContext;

#endif /* __COMMAND_CONTEXT_H__ */
