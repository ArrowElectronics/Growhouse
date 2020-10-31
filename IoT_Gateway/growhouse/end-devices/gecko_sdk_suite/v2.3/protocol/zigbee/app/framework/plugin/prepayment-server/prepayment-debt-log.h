// *****************************************************************************
// * prepayment-debt-log-table.h
// *
// * Implemented routines for interacting with the debt log.
// *
// * Copyright 2014 by Silicon Laboratories, Inc.
// *****************************************************************************

#ifndef SILABS_PREPAYMENT_DEBT_LOG_H
#define SILABS_PREPAYMENT_DEBT_LOG_H

/**
 * @brief Initializes the debt log.
 *
 **/
void emberAfPluginPrepaymentServerInitDebtLog(void);

enum {
  DEBT_TYPE_1  = 0x00,
  DEBT_TYPE_2  = 0x01,
  DEBT_TYPE_3  = 0x02,
  DEBT_TYPE_ALL = 0xFF
};

/**
 * @brief Updates the debt collection attributes for the specified debt index (DEBT_INDEX_1, 2, or 3).
 * @param endpoint The endpoint number of the prepayment server.
 * @param debtIndex The debt index whose collection attributes should be updated.
 *
 **/
void emberAfPluginUpdateDebtOnCollectionEvent(uint8_t endpoint, uint8_t debtType);

/**
 * @brief Prints the information in the debt log at the specified index.
 * @param index The index into the debt log whose contents should be printed.
 *
 **/
void emberAfPluginPrepaymentPrintDebtLogIndex(uint8_t index);

/**
 * @brief Prints the debt attribute set specified by the index parameter.
 * @param endpoint The endpoint number of the prepayment server.
 * @param index Specifies which of the 3 debt attribute sets should be printed.
 *
 **/
void emberAfPluginPrepaymentPrintDebtAttributes(uint8_t endpoint, uint8_t index);

/**
 * @brief Requests sending a Publish Debt Log command.
 * @param nodeId The address of the destination node to whom the Publish Debt Log command should be sent.
 * @param srcEndpoint The endpoint of the sender that supports the prepayment server.
 * @param dstEndpoint The endpoint on the destination node that supports the prepayment client.
 * @param latestEndTime The latest collection time of debt repayment records to be returned in
 * the Publish Debt Log command.
 * @param numberOfDebts The maximum number of debt repayment records to send in the command.
 * @param debtType Identifies the type of debt record(s) to include in the command.
 *
 **/
void emberAfPluginSendPublishDebtLog(EmberNodeId nodeId, uint8_t srcEndpoint, uint8_t dstEndpoint,
                                     uint32_t latestEndTime, uint8_t numberOfDebts, uint8_t debtType);

/**
 * @brief Calculates the percentage of a top up amount that should be applied towards debt reduction.
 * @param endpoint The endpoint number of the prepayment server
 * @param topUpValue The top up value amount.
 * @return Returns the amount of the top up value that should be applied to debt reduction.
 **/
uint32_t emberAfPluginPrepaymentGetDebtRecoveryTopUpPercentage(uint8_t endpoint, uint32_t topUpValue);

#endif  // #ifndef _PREPAYMENT_DEBT_LOG_H_
