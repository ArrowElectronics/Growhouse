// *******************************************************************
// * ota-server-policy.h
// *
// * A sample policy file that implements the callbacks for the
// * Zigbee Over-the-air bootload cluster server.
// *
// * Copyright 2010 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

void emAfOtaServerPolicyPrint(void);

void emAfOtaServerSetQueryPolicy(uint8_t value);
void emAfOtaServerSetBlockRequestPolicy(uint8_t value);
void emAfOtaServerSetUpgradePolicy(uint8_t value);

bool emAfServerPageRequestTickCallback(uint16_t relativeOffset, uint8_t blockSize);
void emAfSetPageRequestMissedBlockModulus(uint16_t modulus);
void emAfOtaServerSetPageRequestPolicy(uint8_t value);
void emAfOtaServerPolicySetMinBlockRequestPeriod(uint16_t minBlockRequestPeriodMS);
uint8_t emberAfOtaServerImageBlockRequestCallback(EmberAfImageBlockRequestCallbackStruct* data);
