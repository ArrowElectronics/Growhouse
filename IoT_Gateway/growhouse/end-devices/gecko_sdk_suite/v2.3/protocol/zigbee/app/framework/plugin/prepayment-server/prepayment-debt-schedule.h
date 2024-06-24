#ifndef SILABS_PREPAYMENT_DEBT_SCHEDULE_H
#define SILABS_PREPAYMENT_DEBT_SCHEDULE_H

typedef struct {
  uint32_t issuerEventId;
  uint16_t collectionTime;
  uint32_t nextCollectionTimeUtc;
  uint32_t firstCollectionTimeSec; // Time of first collection
  uint8_t  collectionFrequency;   // Time between collections (hour, day, week, month, etc)
  uint8_t  debtType;
  uint8_t  endpoint;
} emDebtScheduleEntry;

/**
 * @brief Initializes the debt schedule.
 *
 **/
void emberAfPluginPrepaymentServerInitDebtSchedule(void);

/**
 * @brief Schedules a received debt repayment event.
 * @param endpoint The endpoint number of the prepayment server.
 * @param issuerEventId The issuerEventId sent in the received Change Debt command.
 * @param debtType Indicates if the debt applies to Debt #1, #2, or #3 attributes.
 * @param collectionTime The time offset (in minutes) relative to midnight when debt collection
 * should take place.
 * @param startTime The UTC time that denotes the time at which the debt collection should start,
 * subject to the collectionTime.
 * @param collectionFrequency Specifies the period over which each collection should take place
 * (hourly, daily, weekly, etc).
 *
 **/
void emberAfPluginPrepaymentServerScheduleDebtRepayment(uint8_t endpoint, uint32_t issuerEventId, uint8_t debtType, uint16_t collectionTime, uint32_t startTime, uint8_t collectionFrequency);

/**
 * @brief Checks all debt schedules to see if any debt collections must be performed.
 * @param endpoint The endpoint number of the prepayment server.
 * @param timeNowUtc Specifies the current UTC time.
 *
 **/
void emberAfPrepaymentServerSetDebtMode(uint8_t endpoint, uint32_t timeNowUtc);

/**
 * @brief Determines the number of seconds until the next debt collection event will occur.
 * @param timeNowUtc Specifies the current UTC time.
 *
 **/
uint32_t emberAfPrepaymentServerSecondsUntilDebtCollectionEvent(uint32_t timeNowUtc);

#endif  // #ifndef _PREPAYMENT_DEBT_SCHEDULE_H_
