// *******************************************************************
// * green-power-test-device.c
// *
// *
// * Copyright 2015 by Silicon Laboratories. All rights reserved.           *80*
// * Jeffrey Rosenberger ( jeffrey.rosenberger@silabs.com )
// *******************************************************************

#include "enums.h"
#include "app/framework/include/af.h"
#include "app/framework/util/common.h"
//#include "green-power-client.h"
#include "stack/include/gp-types.h"
//#include "stack/gp/gp-proxy-table.h"
#include "app/framework/plugin/green-power-common/green-power-common.h"

bool __emIsGpTestDevice = true;

void gpSetNetChannel(uint8_t c);
void emberGpepIncomingMessageHandler(GP_PARAMS)
{
  emberAfGreenPowerClusterPrintln("rx gpdf: %1x", gpdCommandId);
  if (gpdCommandId == EMBER_ZCL_GP_GPDF_CHANNEL_CONFIGURATION) {
    emberAfGreenPowerClusterPrintln("got channel conf %1x", gpdCommandPayload[0]);
    //emberAfGreenPowerClusterPrintln("channelthingy = %1x",");

    gpSetNetChannel( (gpdCommandPayload[0] & 0x0F) + 11);
  } else if (gpdCommandId == EMBER_ZCL_GP_GPDF_COMMISSIONING_REPLY) {
    uint8_t i;
    emberAfGreenPowerClusterPrint("got comm rep: ");
    for (i = 0; i < 16; i++) {
      emberAfGreenPowerClusterPrint("%1x", gpdCommandPayload[1 + i]);
    }
    emberAfGreenPowerClusterPrintln("");

    //let's send a success GPDF
    uint8_t testMessage[] =
    {
      0x01, 0x08, 0x5A, 0xFF, 0xFF, 0xFF, 0xFF,// MAC:
      0x0C,    // NWK FCF:
      0x21, 0x43, 0x65, 0x87, // SrcID :
      EMBER_ZCL_GP_GPDF_SUCCESS
    };
    EmberMessageBuffer mesg = emberFillLinkedBuffers(testMessage, 13);

    emberSendRawMessage(mesg);
  }
}
void emberDGpSentHandler()
{
}
