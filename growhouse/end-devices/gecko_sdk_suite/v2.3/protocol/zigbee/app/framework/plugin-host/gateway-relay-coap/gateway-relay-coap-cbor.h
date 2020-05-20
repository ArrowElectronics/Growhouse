// Copyright 2017 Silicon Laboratories, Inc.                                *80*

// CBOR API
EmberStatus emAfPluginGatewayRelayCoapCborReadInit(uint8_t *buffer,
                                                   uint16_t bufferLength);
EmberStatus emAfPluginGatewayRelayCoapCborGetUnsignedInteger(uint32_t *value,
                                                             uint8_t *size);
EmberStatus emAfPluginGatewayRelayCoapCborGetSignedInteger(int32_t *value,
                                                           uint8_t *size);
EmberStatus emAfPluginGatewayRelayCoapCborWriteInit(uint8_t *buffer,
                                                    uint16_t bufferLength);
EmberStatus emAfPluginGatewayRelayCoapCborGetByteStringDefiniteLength(uint8_t *buffer,
                                                                      uint16_t bufferLength);
EmberStatus emAfPluginGatewayRelayCoapCborGetBooleanValue(bool *value);

uint16_t emAfPluginGatewayRelayCoapCborCurrentWriteDataLength(void);
EmberStatus emAfPluginGatewayRelayCoapCborWriteInt8(int8_t signedData8);
EmberStatus emAfPluginGatewayRelayCoapCborWriteInt16(int16_t signedData16);
EmberStatus emAfPluginGatewayRelayCoapCborWriteInt32(int32_t signedData32);
EmberStatus emAfPluginGatewayRelayCoapCborWriteUint8(uint8_t data8);
EmberStatus emAfPluginGatewayRelayCoapCborWriteUint16(uint16_t data16);
EmberStatus emAfPluginGatewayRelayCoapCborWriteUint32(uint32_t data32);
EmberStatus emAfPluginGatewayRelayCoapCborWriteMapTag(void);
EmberStatus emAfPluginGatewayRelayCoapCborWriteArrayTag(void);
EmberStatus emAfPluginGatewayRelayCoapCborWriteBreak(void);
EmberStatus emAfPluginGatewayRelayCoapCborWriteTrue(void);
EmberStatus emAfPluginGatewayRelayCoapCborWriteFalse(void);
EmberStatus emAfPluginGatewayRelayCoapCborWriteString(char *string);
