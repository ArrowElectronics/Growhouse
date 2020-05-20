/***************************************************************************//**
 * @file app_ci.h
 * @brief This is the header file for common RAIL test application commands.
 * @copyright Copyright 2015 Silicon Laboratories, Inc. www.silabs.com
 ******************************************************************************/

#ifndef __APP_CI_H__
#define __APP_CI_H__

/******************************************************************************
 * Application commands.
 *****************************************************************************/
// Base set of Command Interface commands
// <command> <args> <help text>
// u=uint8, v=uint16, w=uint32, s=int32, b=string, ?=Anything, *=0 or more of previous

#define APP_CI_COMMANDS                                                                                                                                                                                         \
  COMMAND_SEPARATOR("Application Configuration"),                                                                                                                                                               \
  COMMAND_ENTRY("setEventConfig", "ww", setEventConfig, "[mask events] Modify RAIL_ConfigEvents with the given mask and events"),                                                                               \
  COMMAND_ENTRY("printDataRates", "", printDataRates, "Print the data rates of the current PHY"),                                                                                                               \
  COMMAND_ENTRY("resetCounters", "", resetCounters, "Resets the Tx and Rx counters"),                                                                                                                           \
  COMMAND_ENTRY("setPeripheralEnable", "u", setPeripheralEnable, "[enable] Enable(1) or Disable(0) LEDs and LCD peripherals"),                                                                                  \
  COMMAND_ENTRY("setNotifications", "u", setNotifications, "[enable] Enable(1) or Disable(0) status prints that happen asynchronously (rxPacket, txEnd, txError)"),                                             \
  COMMAND_ENTRY("getLogLevels", "", getLogLevels, "Get whether notifications are set or peripherals are enabled"),                                                                                              \
  COMMAND_ENTRY("getVersion", "", getVersion, "Get version information."),                                                                                                                                      \
  COMMAND_ENTRY("getVersionVerbose", "", getVersionVerbose, "Get verbose version information."),                                                                                                                \
  COMMAND_ENTRY("setPtiProtocol", "u", setPtiProtocol, "Get version information."),                                                                                                                             \
  COMMAND_SEPARATOR("Receive and Transmit"),                                                                                                                                                                    \
  COMMAND_ENTRY("rx", "w", rx, "[enable] Enable(1) or Disable(0) receive mode"),                                                                                                                                \
  COMMAND_ENTRY("rxAt", "wbwbu*", rxAt, "[start mode end mode rxTransEndSched hardEnd] Configure scheduled receive."),                                                                                          \
  COMMAND_ENTRY("setRxOptions", "w", setRxOptions, "[rxOptionsBitField] Configure receive options, based on RAIL_RX_OPTION defines"),                                                                           \
  COMMAND_ENTRY("tx", "w", tx, "[n] Transmit n packets. If n is 0 transmit infinitely"),                                                                                                                        \
  COMMAND_ENTRY("txWithOptions", "w", txWithOptions, "[n] Transmit n packets with tx options. If n is 0 transmit infinitely"),                                                                                  \
  COMMAND_ENTRY("txAt", "wb*", txAtTime, "[time mode] Transmit a packet at the time and mode specified"),                                                                                                       \
  COMMAND_ENTRY("txAfterRx", "w", txAfterRx, "[time] Schedule a TX for a delay in us after each receive. 0 to disable"),                                                                                        \
  COMMAND_ENTRY("configTxOptions", "u", configTxOptions, "[txOptionsBitfield] Sets the bitmask to be used as the tx options. See #defines starting with \"RAIL_TX_OPTION_\" in rail_types.h"),                  \
  COMMAND_ENTRY("setFixedLength", "v", setFixedLength, "[fixedLength] Configure fixed length"),                                                                                                                 \
  COMMAND_ENTRY("setchannel", "u", setChannel, "[channel] Set the current radio channel"),                                                                                                                      \
  COMMAND_ENTRY("getchannel", "", getChannel, "Get the current radio channel"),                                                                                                                                 \
  COMMAND_ENTRY("setPower", "sb*", setPower, "[power raw] Set the current transmit power in deci dBm, or raw units if 'raw' is specified"),                                                                     \
  COMMAND_ENTRY("getPower", "", getPower, "Get the current transmit power in deci dBm"),                                                                                                                        \
  COMMAND_ENTRY("sweepTxPower", "", sweepTxPower, "Sweep power levels for the current PA and stream at each level."),                                                                                           \
  COMMAND_ENTRY("getRssi", "w*", getRssi, "[wait] Get RSSI in dBm if the receiver is turned on. Optionally specify whether or not to wait for a valid value in case it is initially invalid."),                 \
  COMMAND_ENTRY("sweepPower", "ssw", sweepPower, "[lowPower] [hiPower] [period] Sweep power in square wave fashion. Specify power in deci dBm, period in microseconds."),                                       \
  COMMAND_ENTRY("startAvgRssi", "wu*", startAvgRssi, "[averageTimeUs] [channel] Start AGC RSSI averaging"),                                                                                                     \
  COMMAND_ENTRY("getAvgRssi", "", getAvgRssi, "Get AGC RSSI averaging result."),                                                                                                                                \
  COMMAND_ENTRY("setRssiOffset", "s", setRssiOffset, "Sets the RSSI offset in dBm."),                                                                                                                           \
  COMMAND_ENTRY("getRssiOffset", "", getRssiOffset, "Gets the RSSI offset."),                                                                                                                                   \
  COMMAND_ENTRY("setTxTone", "w", setTxTone, "[enable] Enable(1) or Disable(0) a tone from the radio"),                                                                                                         \
  COMMAND_ENTRY("setTxStream", "w", setTxStream, "[enable] Enable(1) or Disable(0) a PN9 stream from the radio"),                                                                                               \
  COMMAND_ENTRY("status", "", getStatus, "Print the current status counters"),                                                                                                                                  \
  COMMAND_ENTRY("fifoStatus", "", fifoStatus, "Print the current fifo related counters"),                                                                                                                       \
  COMMAND_ENTRY("setTxDelay", "w", setTxDelay, "[delay] Set the inter-packet delay in milliseconds for repeated Tx"),                                                                                           \
  COMMAND_ENTRY("getTxDelay", "", getTxDelay, "Get the inter-packet delay in milliseconds for repeated Tx"),                                                                                                    \
  COMMAND_ENTRY("setTxPayload", "vu*", setTxPayload, "[offset byte0 byte1 ...] Set the packet bytes to be sent"),                                                                                               \
  COMMAND_ENTRY("setTxPayloadQuiet", "vu*", setTxPayloadQuiet, "[offset byte0 byte1 ...] Functions like 'setTxPayload', but outputs less information"),                                                         \
  COMMAND_ENTRY("setTxLength", "v", setTxLength, "[length] Set the number of bytes to load into the FIFO before transmitting. Actual packet length may vary based on radio configuration"),                     \
  COMMAND_ENTRY("printTxPacket", "", printTxPacket, "Print the current Tx data and length"),                                                                                                                    \
  COMMAND_ENTRY("peek", "v*", peekRx, "[number of bytes] [offset] Peek at the start of receive buffer."),                                                                                                       \
  COMMAND_ENTRY("getTime", "", getTime, "Get the current time from the RAIL timebase in microseconds"),                                                                                                         \
  COMMAND_ENTRY("setTime", "w", setTime, "Set the current time in the RAIL timebase in microseconds"),                                                                                                          \
  COMMAND_ENTRY("dataConfig", "bb", dataConfig, "[txMethod rxMethod] Choose between 'pkt' and 'fifo' data methods for RAIL Tx and Rx"),                                                                         \
  COMMAND_ENTRY("setRxFifo", "v", setRxFifo, "[length] Set the receive buffer's length, which is used in both packet mode and FIFO mode. The length cannot be set above RX_BUFFER_SIZE."),                      \
  COMMAND_ENTRY("setTxFifoThreshold", "v", setTxFifoThreshold, "[txFifoThreshold] Set the Tx Fifo Almost Emtpy threshold"),                                                                                     \
  COMMAND_ENTRY("setRxFifoThreshold", "v", setRxFifoThreshold, "[rxFifoThreshold] Set the Rx Fifo Almost Emtpy threshold"),                                                                                     \
  COMMAND_ENTRY("fifoModeTestOptions", "bb", fifoModeTestOptions, "[txFifoManual rxFifoManual] Manual control over RAILTEST fifo actions"),                                                                     \
  COMMAND_ENTRY("rxFifoManualRead", "bvb*", rxFifoManualRead, "[appendedInfo bytesToRead printTiming] Read bytes out of receive fifo and print"),                                                               \
  COMMAND_ENTRY("txFifoManualLoad", "", txFifoManualLoad, "Will attempt to load data into the fifo if there is space"),                                                                                         \
  COMMAND_ENTRY("fifoReset", "bb", fifoReset, "[tx rx] Reset the transmit or receive fifo"),                                                                                                                    \
  COMMAND_ENTRY("abortRxPacket", "w", abortRxPacket, "[abortOffset] Delay after sync word before idling radio."),                                                                                               \
  COMMAND_ENTRY("printTxAcks", "w", printTxAcks, "[printTxAcks] Enable printing of tx ack packets as they happen."),                                                                                            \
  COMMAND_SEPARATOR("Energy Modes and RF Sense"),                                                                                                                                                               \
  COMMAND_ENTRY("sleep", "bw*", sleep, "[EM# [RfSenseUs RfBand]] Sleep in EM# with RFSenseUs on RfBand (0=none,1=2.4GHz,2=SubGHz,3=both) (and UART input)"),                                                    \
  COMMAND_ENTRY("rfsense", "ww", rfSense, "[RfSenseUs RfBand] Start RfSensing with RSenseUs on RfBand"),                                                                                                        \
  COMMAND_SEPARATOR("Address Filtering"),                                                                                                                                                                       \
  COMMAND_ENTRY("configAddressFilter", "wu*", setAddressFilterConfig, "[matchTable offset0 size0 offset1 size1] Configure the addresss filter."),                                                               \
  COMMAND_ENTRY("setAddressFiltering", "u", setAddressFilter, "[enable] Enable(1) or Disable(0) address filtering."),                                                                                           \
  COMMAND_ENTRY("getAddressFiltering", "", getAddressFilter, "Print the current state of address filtering."),                                                                                                  \
  COMMAND_ENTRY("printAddresses", "", printAddresses, "Print the current address filtering addresses."),                                                                                                        \
  COMMAND_ENTRY("setAddress", "uuu*", setAddress, "[field index value...] Set the address value at (field, index) to value."),                                                                                  \
  COMMAND_ENTRY("setAddressEnable", "uuu", enableAddress, "[field index enable] Enable address filtering for the given address."),                                                                              \
  COMMAND_SEPARATOR("Error Rate Testing"),                                                                                                                                                                      \
  COMMAND_ENTRY("perRx", "ww", startPerMode, "[packets delayUs] Start a Packet Error Rate test. 'perRx 0 0' will disable ongoing test."),                                                                       \
  COMMAND_ENTRY("perStatus", "", getPerStats, "Output the results of the PER test. Also see 'status' command"),                                                                                                 \
  COMMAND_ENTRY("setBerConfig", "w", berConfigSet, "[number bytes] Set number of bytes to receive in BER mode; 536870911 = max number of bytes to test; 0 = set max number of bytes to test"),                  \
  COMMAND_ENTRY("berRx", "w", berRx, "[enable] Enable(1) or Disable(0) BER receive mode"),                                                                                                                      \
  COMMAND_ENTRY("berStatus", "", berStatusGet, "Get status of last BER test or of current running test; status information is reset for commands setBerConfig and berRx enable"),                               \
  COMMAND_SEPARATOR("Listen Before Talk (LBT)"),                                                                                                                                                                \
  COMMAND_ENTRY("setLbtMode", "b*", setLbtMode, "[modeStr] Set LBT mode off, csma, lbt"),                                                                                                                       \
  COMMAND_ENTRY("getLbtParams", "", getLbtParams, "Get the current LBT parameters"),                                                                                                                            \
  COMMAND_ENTRY("setLbtParams", "uuusvvw", setLbtParams, "[minBo maxBo tries thresh backoff duration timeout] Set LBT parameters"),                                                                             \
  COMMAND_SEPARATOR("802.15.4 Mode"),                                                                                                                                                                           \
  COMMAND_ENTRY("enable802154", "bvvv", ieee802154Enable, "[defaultState idleTime turnaroundTime ackTimeout] Enable 802.15.4 mode"),                                                                            \
  COMMAND_ENTRY("config2p4GHz802154", "", config2p4Ghz802154, "Configure the radio for 2.4 GHz 802.15.4. This should be called in addition to 'enable802154'."),                                                \
  COMMAND_ENTRY("config863MHz802154", "", config863Mhz802154, "Configure the radio for 863 MHz 802.15.4 GB868. This should be called in addition to 'enable802154'."),                                          \
  COMMAND_ENTRY("config915MHz802154", "", config915Mhz802154, "Configure the radio for 915 MHz 802.15.4 GB868. This should be called in addition to 'enable802154'."),                                          \
  COMMAND_ENTRY("acceptFrames", "uuuu", ieee802154AcceptFrames, "[command ack data beacon] Enable(1) or Disable(0) 802.15.4 frame acceptance. Default settings for 802.15.4 are 1 0 1 1."),                     \
  COMMAND_ENTRY("setPromiscuousMode", "u", ieee802154SetPromiscuousMode, "[enable] Enable(1) or Disable(0) promiscuous mode"),                                                                                  \
  COMMAND_ENTRY("setPanCoordinator", "u", ieee802154SetPanCoordinator, "[enable] Enable(1) or Disable(0) the node acting as a PAN coordinator"),                                                                \
  COMMAND_ENTRY("setPanId802154", "vu*", ieee802154SetPanId, "[panId index] Set the PAN ID for the given index. Index defaults to 0 if not given"),                                                             \
  COMMAND_ENTRY("setShortAddr802154", "vu*", ieee802154SetShortAddress, "[shortAddr index] Set the short address(es) for the given index. Index defaults to 0 if not given"),                                   \
  COMMAND_ENTRY("setLongAddr802154", "uuuuuuuuu*", ieee802154SetLongAddress, "[longAddr_0 ... longAddr_7 index] Set the long address for the given index. Index defaults to 0 if not given"),                   \
  COMMAND_ENTRY("setAddresses802154", "vvb*", ieee802154SetAddresses, "[panId0 shortAddr0 longAddr0 panId1 ... ] Set all 802.15.4 address information."),                                                       \
  COMMAND_SEPARATOR("BLE Mode"),                                                                                                                                                                                \
  COMMAND_ENTRY("setBleMode", "u", bleEnable, "[enable] Set BLE mode to enabled or disabled"),                                                                                                                  \
  COMMAND_ENTRY("getBleMode", "", bleStatus, "Get the current BLE mode"),                                                                                                                                       \
  COMMAND_ENTRY("setBleChannelParams", "uw*", bleSetChannelParams, "[logicalChannel accessAddr crcInit disableWhiten] Configure channel parameters related to BLE"),                                            \
  COMMAND_ENTRY("setBleAdvertising", "u", bleAdvertisingConfig, "[advChannel] Configure for BLE advertising on channel 37, 38, or 39"),                                                                         \
  COMMAND_ENTRY("setBle1Mbps", "u*", bleSet1MbpsPhy, "[isViterbi] Switch to the 1Mbps BLE PHY"),                                                                                                                \
  COMMAND_ENTRY("setBle2Mbps", "u*", bleSet2MbpsPhy, "[isViterbi] Switch to the 2Mbps BLE PHY"),                                                                                                                \
  COMMAND_ENTRY("setBleCoding", "u", bleSetCoding, "[coding] Switch to the given RAIL_BLE_Coding_t value"),                                                                                                     \
  COMMAND_SEPARATOR("RAIL Timer"),                                                                                                                                                                              \
  COMMAND_ENTRY("setTimer", "wb", setTimer, "[timeout mode] Set the RAIL timer timeout. You can use either an absolute (abs) or relative (rel) timer mode."),                                                   \
  COMMAND_ENTRY("timerCancel", "", timerCancel, "Cancel the RAIL timer if it's active."),                                                                                                                       \
  COMMAND_ENTRY("printTimerStats", "", printTimerStats, "Print current timer configuration."),                                                                                                                  \
  COMMAND_SEPARATOR("Auto Acking"),                                                                                                                                                                             \
  COMMAND_ENTRY("autoAckConfig", "bvvv", autoAckConfig, "[defaultState idleTime turnaroundTime ackTimeout] Configure and enable auto ack functionality in RAIL. "),                                             \
  COMMAND_ENTRY("autoAckDisable", "", autoAckDisable, "Disable auto ack. Use autoAckConfig to reenable."),                                                                                                      \
  COMMAND_ENTRY("setAckPayload", "vu*", setAckPayload, "[offset byte0 byte1 ...] Set the ack bytes to be sent."),                                                                                               \
  COMMAND_ENTRY("setAckLength", "v", setAckLength, "[length] Set the number of bytes to transmit for ack payloads"),                                                                                            \
  COMMAND_ENTRY("printAckPacket", "", printAckPacket, "Print the current ack data and length"),                                                                                                                 \
  COMMAND_ENTRY("getAutoAck", "", getAutoAck, "Print the current state of auto acking."),                                                                                                                       \
  COMMAND_ENTRY("autoAckPause", "uu", autoAckPause, "[RxPause TxPause] Pause(1) or Resume(0) Auto Acking"),                                                                                                     \
  COMMAND_ENTRY("setTxAckOptions", "uu", setTxAckOptions, "[cancelAck useTxBuf] Enable(1) or Disable(0) feature for one receive"),                                                                              \
  COMMAND_SEPARATOR("Diagnostic and Test"),                                                                                                                                                                     \
  COMMAND_ENTRY("getmemw", "ww*", getMemWord, "[address count] Read count 32bit words starting at address"),                                                                                                    \
  COMMAND_ENTRY("setmemw", "ww*", setMemWord, "[address value...] Write as many 32bit values as specified starting at address"),                                                                                \
  COMMAND_ENTRY("setCtune", "v", setCtune, "[ctune] Set the value of CTUNE in the CMU->HFXOSTEADYSTATECTRL register"),                                                                                          \
  COMMAND_ENTRY("getCtune", "", getCtune, "Get the value of CTUNE in the CMU->HFXOSTEADYSTATECTRL register"),                                                                                                   \
  COMMAND_ENTRY("setPaCtune", "uu", setPaCtune, "[txPaCtune] [rxPaCtune] Set the value of PACTUNE for TX and RX mode"),                                                                                         \
  COMMAND_ENTRY("enablePaCal", "u", enablePaCal, "[enable] Enable(1) or Disable(0) PA power calibration"),                                                                                                      \
  COMMAND_ENTRY("setDebugSignal", "?", setDebugSignal, "Configure chip specific debug output. Use 'setDebugSignal help' for more details."),                                                                    \
  COMMAND_ENTRY("setDebugMode", "w", setDebugMode, "[mode] 1 = Frequency Override. 0 = Disable debug mode"),                                                                                                    \
  COMMAND_ENTRY("freqOverride", "w", setFrequency, "[freq] Change to freq specified in Hz. Requires debug mode to be enabled. Only small frequency deviations from the current configuration are supported."),  \
  COMMAND_ENTRY("directMode", "u", setDirectMode, "[enable] Enable(1) or Disable(0) direct mode"),                                                                                                              \
  COMMAND_ENTRY("directTx", "u", setDirectTx, "[enable] Enable(1) or Disable(0) TX in direct mode"),                                                                                                            \
  COMMAND_ENTRY("txCancel", "s", txCancel, "[delay] Set the time in microseconds after which we should cancel a tx (a negative value disables this)"),                                                          \
  COMMAND_ENTRY("getRandom", "vu*", getRandom, "[len hidden] Get len bytes of random data from the radio. Only print them to the screen if hidden is 0 (default)."),                                            \
  COMMAND_ENTRY("setTxUnderflow", "w", setTxUnderflow, "[enable] Enable(1) or Disable(0) TX underflows"),                                                                                                       \
  COMMAND_ENTRY("setRxOverflow", "w*", setRxOverflow, "[enable delayUs] Enable(1) or Disable(0) RX overflows. The overflow will be caused by hanging in the interrupt handler for delayUs"),                    \
  COMMAND_ENTRY("setCalibrations", "w", setCalibrations, "[enable] Enable(1) or Disable(0) RAIL calibrations"),                                                                                                 \
  COMMAND_ENTRY("setTxTransitions", "bb", setTxTransitions, "[txSuccess txError] Set each RAIL TX state transition value to r(x) or i(dle)"),                                                                   \
  COMMAND_ENTRY("setRxTransitions", "bb", setRxTransitions, "[rxSuccess rxError] Set each RAIL RX state transition value to t(x), r(x), or i(dle)."),                                                           \
  COMMAND_ENTRY("setTimings", "vvvv*", setTimings, "[idleToRx txToRx idleToTx rxToTx rxSearch txToRxSearch] Set RAIL state transition timings in microseconds"),                                                \
  COMMAND_ENTRY("forceAssert", "w", forceAssert, "[errorCode] Force a RAIL assert with the given error code."),                                                                                                 \
  COMMAND_ENTRY("printEvents", "w", configPrintEvents, "[printEvents] Enable printing of RAIL events in chronological order."),                                                                                 \
  COMMAND_ENTRY("getAppMode", "", getAppMode, "Print the current app mode of RAILTEST. Values printed are those to be found in AppMode_t."),                                                                    \
  COMMAND_ENTRY("getRadioState", "", getRadioState, "Get the RAIL radio state. Values returned correspond to RAIL_RadioState_t."),                                                                              \
  COMMAND_ENTRY("verifyRadio", "wuuu", verifyRadio, "[durationUs restart override callback] Verify radio memory contents and return after duration in microseconds. Restart (1) or resume (0) from last run."), \
  COMMAND_ENTRY("reset", "", resetChip, "Perform a reboot of the chip")

/******************************************************************************
 * Externed Prototypes
 *****************************************************************************/
// Command callback function prototypes
// trx_ci
void rx(int argc, char **argv);
void rxAt(int argc, char **argv);
void setRxOptions(int argc, char **argv);
void tx(int argc, char **argv);
void txWithOptions(int argc, char **argv);
void configTxOptions(int argc, char **argv);
void txAtTime(int argc, char **argv);
void txAfterRx(int argc, char **argv);
void setTxTone(int argc, char **argv);
void setTxStream(int argc, char **argv);
void setDirectMode(int argc, char **argv);
void setDirectTx(int argc, char **argv);
void sleep(int argc, char **argv);
void rfSense(int argc, char **argv);
void printTxAcks(int argc, char **argv);

//lbt_ci
void setLbtMode(int argc, char **argv);
void getLbtParams(int argc, char **argv);
void setLbtParams(int argc, char **argv);

//packet_ci
void setTxPayload(int argc, char **argv);
void setTxPayloadQuiet(int argc, char **argv);
void setTxLength(int argc, char **argv);
void printTxPacket(int argc, char **argv);
void peekRx(int argc, char **argv);

void setAckPayload(int argc, char **argv);
void setAckLength(int argc, char **argv);
void printAckPacket(int argc, char **argv);

void setFixedLength(int argc, char **argv);
void dataConfig(int argc, char **argv);
void setRxFifo(int argc, char **argv);
void fifoReset(int argc, char **argv);
void fifoModeTestOptions(int argc, char **argv);
void rxFifoManualRead(int argc, char **argv);
void txFifoManualLoad(int argc, char **argv);
void abortRxPacket(int argc, char **argv);

//parameter_ci
void setChannel(int argc, char **argv);
void getChannel(int argc, char **argv);
void setDirectMode(int argc, char **argv);
void setDirectTx(int argc, char **argv);
void getMemWord(int argc, char **argv);
void setMemWord(int argc, char **argv);

void getPower(int argc, char **argv);
void setPower(int argc, char **argv);
void sweepTxPower(int argc, char **argv);
void getTxDelay(int argc, char **argv);
void setTxDelay(int argc, char **argv);
void setTxTransitions(int argc, char **argv);
void setRxTransitions(int argc, char **argv);
void setTimings(int argc, char **argv);

void setCtune(int argc, char **argv);
void getCtune(int argc, char **argv);

void setPaCtune(int argc, char **argv);
void enablePaCal(int argc, char **argv);

void setTxFifoThreshold(int argc, char **argv);
void setRxFifoThreshold(int argc, char **argv);

void setEventConfig(int argc, char **argv);

//info_ci
void getStatus(int argc, char **argv);
void fifoStatus(int argc, char **argv);
void getVersion(int argc, char **argv);
void getVersionVerbose(int argc, char **argv);
void setPtiProtocol(int argc, char **argv);
void getRssi(int argc, char **argv);
void sweepPower(int argc, char **argv);
void startAvgRssi(int argc, char **argv);
void getAvgRssi(int argc, char **argv);
void setRssiOffset(int agrc, char **argv);
void getRssiOffset(int agrc, char **argv);
void resetCounters(int argc, char **argv);
void getTime(int argc, char **argv);
void setTime(int argc, char **argv);

//debug_ci
void setFrequency(int argc, char **argv);
void setDebugMode(int argc, char **argv);
void getMemWord(int argc, char **argv);
void setMemWord(int argc, char **argv);
void setTxUnderflow(int argc, char **argv);
void setRxOverflow(int argc, char **argv);
void setCalibrations(int argc, char **argv);
void getLogLevels(int argc, char **argv);
void setPeripheralEnable(int argc, char **argv);
void setNotifications(int argc, char **argv);
void resetChip(int argc, char **argv);
void printDataRates(int argc, char **argv);
void txCancel(int argc, char **argv);
void getRandom(int argc, char **argv);
void setDebugSignal(int argc, char **argv);
void forceAssert(int argc, char**argv);
void configPrintEvents(int argc, char**argv);
void getAppMode(int argc, char**argv);
void getRadioState(int argc, char**argv);
void verifyRadio(int argc, char**argv);

//address_filter_ci
void setAddressFilterConfig(int argc, char **argv);
void setAddressFilter(int argc, char **argv);
void getAddressFilter(int argc, char **argv);
void printAddresses(int argc, char **argv);
void setAddress(int argc, char **argv);
void enableAddress(int argc, char **argv);

//error_rate_ci
void startPerMode(int argc, char **argv);
void getPerStats(int argc, char **argv);
void berConfigSet(int argc, char **argv);
void berRx(int argc, char **argv);
void berStatusGet(int argc, char **argv);

// 154_rx_ci
void ieee802154Enable(int argc, char **argv);
void config2p4Ghz802154(int argc, char **argv);
void config863Mhz802154(int argc, char **argv);
void config915Mhz802154(int argc, char **argv);
void ieee802154AcceptFrames(int argc, char **argv);
void ieee802154SetPromiscuousMode(int argc, char **argv);
void ieee802154SetPanCoordinator(int argc, char **argv);
void ieee802154SetPanId(int argc, char **argv);
void ieee802154SetShortAddress(int argc, char **argv);
void ieee802154SetLongAddress(int argc, char **argv);
void ieee802154SetAddresses(int argc, char **argv);

// ble_ci
void bleEnable(int argc, char **argv);
void bleStatus(int argc, char **argv);
void bleSetChannelParams(int argc, char **argv);
void bleAdvertisingConfig(int argc, char **argv);
void bleSet1MbpsPhy(int argc, char **argv);
void bleSet2MbpsPhy(int argc, char **argv);
void bleSetCoding(int argc, char **argv);

//timer_ci
void setTimer(int argc, char **argv);
void printTimerStats(int argc, char **argv);
void timerCancel(int argc, char** argv);

//autoack_ci
void autoAckConfig(int argc, char **argv);
void autoAckDisable(int argc, char **argv);
void getAutoAck(int argc, char **argv);
void autoAckPause(int argc, char **argv);
void setTxAckOptions(int argc, char **argv);

#endif // __APP_CI_H__
