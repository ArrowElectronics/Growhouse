// Copyright 2007 - 2012 by Ember Corporation. All rights reserved.
//
//

#include "app/framework/include/af.h"
#include "diagnostic-server.h"

// This is just a stub for the host processor
bool emberAfReadDiagnosticAttribute(
  EmberAfAttributeMetadata *attributeMetadata,
  uint8_t *buffer)
{
  return false;
}

bool emberAfPreMessageReceivedCallback(EmberAfIncomingMessage* incomingMessage)
{
  return false;
}
