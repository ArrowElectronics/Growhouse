// *******************************************************************
// * af-node.c
// *
// * Security code for a normal (non-Trust Center) node.
// *
// * Copyright 2009 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#include "app/framework/include/af.h"
#define USE_REAL_SECURITY_PROTOTYPES
#include "app/framework/security/af-security.h"

//------------------------------------------------------------------------------

EmberStatus zaNodeSecurityInit(bool centralizedNetwork)
{
  EmberInitialSecurityState state;
  EmberExtendedSecurityBitmask newExtended;
  EmberExtendedSecurityBitmask oldExtended;
  EmberStatus status;
  const EmberAfSecurityProfileData *data = emAfGetCurrentSecurityProfileData();

  if (data == NULL) {
    return EMBER_ERR_FATAL;
  }

  MEMSET(&state, 0, sizeof(EmberInitialSecurityState));
  state.bitmask = data->nodeBitmask;
  newExtended = data->nodeExtendedBitmask;
  MEMMOVE(emberKeyContents(&state.preconfiguredKey),
          emberKeyContents(&data->preconfiguredKey),
          EMBER_ENCRYPTION_KEY_SIZE);

#if defined ZA_CLI_FULL
  // This function will only be used if the full CLI is enabled,
  // and a value has been previously set via the "changekey" command.
  getLinkKeyFromCli(&(state.preconfiguredKey));
#endif

  emberGetExtendedSecurityBitmask(&oldExtended);
  if ((oldExtended & EMBER_EXT_NO_FRAME_COUNTER_RESET) != 0U) {
    newExtended |= EMBER_EXT_NO_FRAME_COUNTER_RESET;
  }

  if (!centralizedNetwork) {
    state.bitmask |= EMBER_DISTRIBUTED_TRUST_CENTER_MODE;
  }

  emberAfSecurityInitCallback(&state, &newExtended, false); // trust center?

  emberAfSecurityPrintln("set state to: 0x%2x", state.bitmask);
  status = emberSetInitialSecurityState(&state);
  if (status != EMBER_SUCCESS) {
    emberAfSecurityPrintln("security init node: 0x%x", status);
    return status;
  }

  // Don't need to check on the status here, emberSetExtendedSecurityBitmask
  // always returns EMBER_SUCCESS.
  emberAfSecurityPrintln("set extended security to: 0x%2x", newExtended);
  emberSetExtendedSecurityBitmask(newExtended);

  emAfClearLinkKeyTable();

  return EMBER_SUCCESS;
}
