#ifndef SILABS_PREPAYMENT_CLIENT_H
#define SILABS_PREPAYMENT_CLIENT_H

void emberAfPluginPrepaymentClientChangePaymentMode(EmberNodeId nodeId, uint8_t srcEndpoint, uint8_t dstEndpoint, uint32_t providerId, uint32_t issuerEventId, uint32_t implementationDateTime, uint16_t proposedPaymentControlConfiguration, uint32_t cutOffValue);

bool emberAfPluginPrepaymentClusterChangePaymentModeResponseCallback(uint8_t friendlyCredit, uint32_t friendlyCreditCalendarId, uint32_t emergencyCreditLimit, uint32_t emergencyCreditThreshold);

#endif  // #ifndef _PREPAYMENT_CLIENT_H_
