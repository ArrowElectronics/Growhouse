// *******************************************************************
// * mn-price-passthrough.h
// *
// *
// * Copyright 2012 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

void emAfPluginMnPricePassthroughStartPollAndForward(void);
void emAfPluginMnPricePassthroughStopPollAndForward(void);
void emAfPluginMnPricePassthroughRoutingSetup(EmberNodeId fwdId,
                                              uint8_t fwdEndpoint,
                                              uint8_t esiEndpoint);
void emAfPluginMnPricePassthroughPrintCurrentPrice(void);

#ifndef EMBER_AF_PLUGIN_PRICE_SERVER
/**
 * @brief The price and metadata used by the MnPricePassthrough plugin.
 *
 */
typedef struct {
  uint32_t  providerId;
  uint8_t   rateLabel[ZCL_PRICE_CLUSTER_MAXIMUM_RATE_LABEL_LENGTH + 1];
  uint32_t  issuerEventID;
  uint8_t   unitOfMeasure;
  uint16_t  currency;
  uint8_t   priceTrailingDigitAndTier;
  uint8_t   numberOfPriceTiersAndTier; // Added later in errata
  uint32_t  startTime;
  uint16_t  duration; // In minutes
  uint32_t  price;
  uint8_t   priceRatio;
  uint32_t  generationPrice;
  uint8_t   generationPriceRatio;
  uint32_t  alternateCostDelivered;
  uint8_t   alternateCostUnit;
  uint8_t   alternateCostTrailingDigit;
  uint8_t   numberOfBlockThresholds;
  uint8_t   priceControl;
} EmberAfScheduledPrice;

#define ZCL_PRICE_CLUSTER_RESERVED_MASK              0xFE
#define ZCL_PRICE_CLUSTER_DURATION_UNTIL_CHANGED 0xFFFF

#endif // EMBER_AF_PLUGIN_PRICE_SERVER
