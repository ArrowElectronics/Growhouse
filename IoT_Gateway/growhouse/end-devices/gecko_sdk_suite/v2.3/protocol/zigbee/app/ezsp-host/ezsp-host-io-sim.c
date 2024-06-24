/** @file ezsp-host-io-sim.c
 *
 *
 */

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/error.h"
#include "hal/hal.h"
#include "hal/micro/generic/ash-protocol.h"
#include "app/ezsp-host/ezsp-host-io.h"
#include "plugin/serial/serial.h"

#define WR_BAD_RANDOM        400  // corrupt write data with 1/N probablility
#define RD_BAD_RANDOM        400  // corrupt read data with 1/N probablility

EzspStatus ezspSerialInit(void)
{
  emberSerialInit(1, BAUD_115200, PARITY_NONE, 1);
  return EZSP_SUCCESS;
}

void ezspSerialWriteByte(uint8_t byte)
{
#ifdef WR_BAD_RANDOM
  uint16_t n = halCommonGetRandom();
  if (n < 0xFFFF / WR_BAD_RANDOM) {
    if (n & 8) {
      byte = ASH_SUB;
    } else {
      byte ^= (1 << (n & 7));
    }
  }
#endif
  (void)emberSerialWriteByte(1, byte);
}

EzspStatus ezspSerialWriteAvailable(void)
{
  if (emberSerialWriteAvailable(1) > 0) {
    return EZSP_SUCCESS;
  } else {
    return EZSP_NO_TX_SPACE;
  }
}

EzspStatus ezspSerialReadByte(uint8_t *byte)
{
  uint8_t ch;
  if (emberSerialReadByte(1, &ch) == EMBER_SUCCESS) {
    *byte = ch;
#ifdef RD_BAD_RANDOM
    uint16_t n = halCommonGetRandom();
    if (n < 0xFFFF / RD_BAD_RANDOM) {
      if (n & 8) {
        *byte = ASH_SUB;
      } else {
        *byte ^= (1 << (n & 7));
      }
      if ( (*byte == ASH_XON) || (*byte == ASH_XOFF) ) {
        (*byte)++;
      }
    }
#endif
    return EZSP_SUCCESS;
  } else {
    return EZSP_NO_RX_DATA;
  }
}

EzspStatus ezspSerialReadAvailable(uint16_t *count)
{
  *count = emberSerialReadAvailable(1);

  if (*count) {
    return EZSP_SUCCESS;
  } else {
    return EZSP_NO_RX_DATA;
  }
}

void ezspSerialReadFlush(void)
{
  uint8_t byte;
  while (ezspSerialReadByte(&byte) == EZSP_SUCCESS) {
  }
}

void ezspSerialWriteFlush(void)
{
}

void ezspDebugFlush(void)
{
}

void ezspResetDtr(void)
{
}

void ezspResetCustom(void)
{
}

void ezspSerialClose(void)
{
}

bool ezspSerialOutputIsIdle(void)
{
  return true;
}

EzspStatus ezspSetupSerialPort(int* serialPortFdReturn,
                               char* errorStringLocation,
                               int maxErrorLength,
                               bool bootloaderMode)
{
  return EZSP_ERROR_INVALID_CALL;
}
