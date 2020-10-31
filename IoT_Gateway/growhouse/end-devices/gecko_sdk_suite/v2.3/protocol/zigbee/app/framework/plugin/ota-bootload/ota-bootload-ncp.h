// ****************************************************************************
// * ota-bootload-ncp.h
// *
// * SPI/UART Interface to bootloading the NCP from a connected host.
// *
// * Copyright 2010 by Ember Corporation. All rights reserved.              *80*
// *****************************************************************************

bool emAfStartNcpBootloaderCommunications(void);
void emAfPostNcpBootload(bool success);
bool emAfRebootNcpAfterBootload(void);

// These primitives are called by the Xmodem code to pass data via
// the specific mechanism (UART or SPI).
bool emAfBootloadSendData(const uint8_t *data, uint16_t length);
bool emAfBootloadSendByte(uint8_t byte);
bool emAfBootloadWaitChar(uint8_t *data, bool expect, uint8_t expected);
