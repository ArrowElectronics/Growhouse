// *******************************************************************
// * common.h
// *
// * This file contains the includes that are common to all clusters
// * in the util.
// *
// * Copyright 2007 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#ifndef SILABS_COMMON_H
#define SILABS_COMMON_H

// App framework
#include "app/framework/include/af.h"
#include "app/framework/util/util.h"
#include "app/framework/util/time-util.h"
#include "app/framework/util/attribute-table.h"
#include "app/framework/util/attribute-storage.h"

// the variables used to setup and send responses to cluster messages
extern EmberApsFrame emberAfResponseApsFrame;
extern uint8_t  appResponseData[EMBER_AF_RESPONSE_BUFFER_LEN];
extern uint16_t appResponseLength;
extern EmberNodeId emberAfResponseDestination;

#endif //SILABS_COMMON_H
