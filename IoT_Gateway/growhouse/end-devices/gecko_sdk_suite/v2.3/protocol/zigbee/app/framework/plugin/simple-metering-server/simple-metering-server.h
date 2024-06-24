/*
 * simple-metering-server.h
 *
 *  Created on: Dec 11, 2014
 *      Author: romacdon
 */

#ifndef SIMPLE_METERING_SERVER_H_
#define SIMPLE_METERING_SERVER_H_

void emAfToggleFastPolling(uint8_t enableFastPolling);
void emberAfPluginSimpleMeteringClusterReadAttributesResponseCallback(EmberAfClusterId clusterId,
                                                                      uint8_t *buffer,
                                                                      uint16_t bufLen);
uint16_t emberAfPluginSimpleMeteringServerStartSampling(uint16_t requestedSampleId,
                                                        uint32_t issuerEventId,
                                                        uint32_t startSamplingTime,
                                                        uint8_t sampleType,
                                                        uint16_t sampleRequestInterval,
                                                        uint16_t maxNumberOfSamples,
                                                        uint8_t endpoint);

#endif /* SIMPLE_METERING_SERVER_H_ */
