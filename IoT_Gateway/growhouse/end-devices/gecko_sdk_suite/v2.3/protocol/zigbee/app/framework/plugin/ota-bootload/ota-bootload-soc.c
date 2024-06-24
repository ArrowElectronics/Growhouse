// ****************************************************************************
// * ota-bootload-soc.c
// *
// * Routines for bootloading an SOC.
// *
// * Copyright 2010 by Ember Corporation. All rights reserved.              *80*
// *****************************************************************************

#include "app/framework/include/af.h"

//------------------------------------------------------------------------------
// Globals

//------------------------------------------------------------------------------

uint8_t emberAfOtaBootloadCallback(const EmberAfOtaImageId* id,
                                   uint16_t ncpUpgradeTagId)
{
  halAppBootloaderInstallNewImage();

  emberAfCorePrintln("Bootloading failed!");

  // If we got here, it is a failure.
  return 1;
}
