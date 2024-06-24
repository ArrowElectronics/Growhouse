// *******************************************************************
//  mfg-sample.c
//
//  sample app for using the mfglib for em250 and em2420
//
//  This is an example of an app that uses the mfglib test framework.
//  It has a very simple serial interface to invoke all provided API.
//
//  Copyright 2016 Silicon Laboratories, Inc.
// *******************************************************************

#include PLATFORM_HEADER //compiler/micro specifics, types
#include "stack/include/ember.h"
#include "stack/include/mfglib.h"
#include "hal/hal.h"
#include "plugin/serial/serial.h"
#include "app/util/serial/cli.h"

// Application serial port
#ifndef APP_SERIAL // Why is this one C file specifying APP_SERIAL??
#define APP_SERIAL 1
#endif//APP_SERIAL

// the following variables are used by the application to keep track of
// what the current application state is
bool mfgLibRunning = false;
bool mfgToneTestRunning = false;
bool mfgStreamTestRunning = false;
bool stackUp = false;

// this variable keeps track of how many mfglib packets have been received
// during a mfglib session (from mfglib start to mfglib end). This can be
// accessed using the "info" command.
uint16_t  mfgTotalPacketCounter;

// this buffer is filled with the contents to be sent using mfglibSendPacket
#pragma align sendBuff
uint8_t   sendBuff[128];

// Function prototypes
void mfglibRxHandler(uint8_t *packet,
                     uint8_t linkQuality,
                     int8_t rssi);
// *******************************************************************
// Ember endpoint and interface configuration

#define ENDPOINT 1
#define PROFILE_ID 0xC00F

EmberEndpointDescription PGM endpointDescription = {
  PROFILE_ID, 0,
};

uint8_t emberEndpointCount = 1;
EmberEndpoint emberEndpoints[] = {
  { ENDPOINT, &endpointDescription, }
};

// End Ember endpoint and interface configuration
// *******************************************************************

// *******************************************************************
// MFGLIB callback function:

// mfglibRxHandler is called whenever a mfglib packet is received if
// the device has called mfglibStart and pass in the function pointer
// as the parameter.
//
// If the mfglib packets are coming in very fast, the receiving node
// should take care to not do a lot of work in mfglibRxHandler.
//
// This example application simply keeps track of the first packet received
// and how many have been received until it sees a pause in sending.
// If it does not receive packets for two "heartbeat" periods (200 ms - see
// the heartBeat function) it considers the stream of packets to have ended
// and prints the results for the user by calling appMfglibSendIsComplete
//
// The following variables relate to keeping the saved packet info and keeping
// track of the state (current packets received in this group, how many
// had been received since last heartbeat call.

// the number of packets in the current transmit group
uint16_t mfgCurrentPacketCounter = 0;

// the saved information for the first packet
uint8_t savedPktLength = 0;
int8_t savedRssi = 0;
uint8_t savedLinkQuality = 0;
uint8_t savedPkt[128];

// if we are in a transmit group of packets or not
bool inReceivedStream = false;

// this keeps track of the number of packets received as of the last heartbeat
// "tick" (200 ms). If this value stays constant for two ticks, then
// appMfglibSendIsComplete is called by heartBeat.
uint16_t heartbeatLastPacketCounterValue = 0;

// *****************************
// mfglibRxHandler
//
// The function is passed to mfglibStart() to report a mfglib message received.
// The first byte of the packet is the length byte.
// *****************************
void mfglibRxHandler(uint8_t *packet,
                     uint8_t linkQuality,
                     int8_t rssi)
{
  // This increments the total packets for the whole mfglib session
  // this starts when mfglibStart is called and stops when mfglibEnd
  // is called.
  mfgTotalPacketCounter++;

  // This keeps track of the number of packets in the current transmit group.
  // This starts when a mfglib packet is received and ends when no mfglib
  // packets are received for two heartBeat ticks (a tick is 200 ms)
  mfgCurrentPacketCounter++;

  // If this is the first packet of a transmit group then save the information
  // of the current packet. Don't do this for every packet, just the first one.
  if (!inReceivedStream) {
    inReceivedStream = true;
    mfgCurrentPacketCounter = 1;
    savedRssi = rssi;
    savedLinkQuality = linkQuality;
    savedPktLength = *packet;
    MEMMOVE(savedPkt, (packet + 1), savedPktLength);
  }
}

// This function is called when a transmit group has completed, which means
// the node has received a mfglib packet and it has been at least two heartBeat
// ticks since it has seen another mfglib packet. This function prints the
// saved info of the first packet in the group and prints the number of
// packets seen.
void appMfglibSendIsComplete(void)
{
  uint8_t i;

  // print the number of packets and the saved info for the first one
  emberSerialPrintf(APP_SERIAL,
                    "\r\nMFG RX %2x pkts\r\n", mfgCurrentPacketCounter);
  emberSerialPrintf(APP_SERIAL, "first packet: lqi %u, rssi %d, len %u\r\n",
                    savedLinkQuality, savedRssi, savedPktLength);
  emberSerialWaitSend(APP_SERIAL);

  // print the data of the packet in rows of 16 bytes
  for (i = 0; i < savedPktLength; i++) {
    if ((i % 16) == 0) {
      emberSerialPrintf(APP_SERIAL, "      : ");
      emberSerialWaitSend(APP_SERIAL);
    }
    emberSerialPrintf(APP_SERIAL, "%x ", savedPkt[i]);
    if ((i % 16) == 15) {
      emberSerialPrintf(APP_SERIAL, "\r\n");
    }
  }
  emberSerialPrintf(APP_SERIAL, "\r\n");
  emberSerialWaitSend(APP_SERIAL);

  // reset the state
  inReceivedStream = false;
  mfgCurrentPacketCounter = 0;
  heartbeatLastPacketCounterValue = 0;
}

//----------------------------------------------------------------

// *****************************
// heartBeat
//
// This blinks the LEDs to let the user know the application is active and
// checks if a mfglib transmit group has completed.
// *****************************
static void heartBeat(void)
{
  static uint16_t lastBlinkTime = 0;
  uint16_t time;
  static uint8_t numTimesPacketCountStayedSame = 0;

  time = halCommonGetInt16uMillisecondTick();

  // make a tick be 200 ms
  if ((uint16_t)(time - lastBlinkTime) > 200 /*ms*/) {
    // **************
    // blink the LED
    halToggleLed(BOARD_HEARTBEAT_LED);
    lastBlinkTime = time;

    // **************
    // check if we are in a transmit group. This means at least one
    // mfglib packet has been received.
    if (inReceivedStream) {
      // check if the value of mfgCurrentPacketCounter (saved in
      // heartbeatLastPacketCounterValue) is the same as the last time we
      // checked (the last tick)
      if (heartbeatLastPacketCounterValue == mfgCurrentPacketCounter) {
        numTimesPacketCountStayedSame++;

        // if the number of received mfglib messages has not incremented
        // within two ticks, then this group has ended.
        if (numTimesPacketCountStayedSame == 2) {
          appMfglibSendIsComplete();
        }
      } else {
        // else means the value of mfgCurrentPacketCounter changed
        numTimesPacketCountStayedSame = 0;
      }
      // remember the current value for mfgCurrentPacketCounter
      heartbeatLastPacketCounterValue = mfgCurrentPacketCounter;
    }
  }
}

//----------------------------------------------------------------

int main(void)
{
  uint8_t reset;
  EmberStatus status;

  // Determine the cause of the reset (powerup, etc).
  reset = halGetResetInfo();

  halInit(); // Initialize the micro, board peripherals, etc

  INTERRUPTS_ON();  // Safe to enable interrupts at this point

  // inititialize the serial port
  // good to do this before emberInit, that way any errors that occur
  // can be printed to the serial port.
  emberSerialInit(APP_SERIAL, BAUD_115200, PARITY_NONE, 1);

  emberSerialGuaranteedPrintf(APP_SERIAL, "Reset(0x%x):%p\r\n",
                              reset, halGetResetString());

  // initialize ember stack
  status = emberInit();
  if (status != EMBER_SUCCESS) {
    // report status here
    emberSerialGuaranteedPrintf(APP_SERIAL,
                                "ERROR: emberInit 0x%x:", status);
    emberSerialGuaranteedPrintf(APP_SERIAL, "\r\n");
  } else {
    emberSerialGuaranteedPrintf(APP_SERIAL, "emberInit passed\r\n");
  }

  // initialize the cli library to use the correct serial port.
  cliInit(APP_SERIAL);

  while (1) {
    halResetWatchdog();      // Periodically reset the watchdog
    emberTick();             // Allow the stack to run
    heartBeat();             // blink the LEDs
    cliProcessSerialInput(); // process serial input from ths user
  }
}

// this is called when the stack status changes
void emberStackStatusHandler(EmberStatus status)
{
  switch (status) {
    case EMBER_NETWORK_UP:
      emberSerialPrintf(APP_SERIAL,
                        "EVENT: stackStatus now EMBER_NETWORK_UP\r\n");
      stackUp = true;
      break;

    case EMBER_NETWORK_DOWN:
      emberSerialPrintf(APP_SERIAL,
                        "EVENT: stackStatus now EMBER_NETWORK_DOWN\r\n");
      stackUp = false;
      break;

    case EMBER_JOIN_FAILED:
      emberSerialPrintf(APP_SERIAL,
                        "EVENT: stackStatus now EMBER_JOIN_FAILED\r\n");
      stackUp = false;
      break;

    default:
      emberSerialPrintf(APP_SERIAL, "EVENT: stackStatus now 0x%x\r\n", status);
  }
}

// this is called when a scan is complete
void emberScanCompleteHandler(uint8_t channel, EmberStatus status)
{
}

// this is called when a network is found when app is performing scan
void emberNetworkFoundHandler(EmberZigbeeNetwork *networkFound)
{
}

// Called when a message has completed transmission --
// status indicates whether the message was successfully
// transmitted or not.
void emberMessageSentHandler(EmberOutgoingMessageType type,
                             uint16_t indexOrDestination,
                             EmberApsFrame *apsFrame,
                             EmberMessageBuffer message,
                             EmberStatus status)
{
}

void emberIncomingMessageHandler(EmberIncomingMessageType type,
                                 EmberApsFrame *apsFrame,
                                 EmberMessageBuffer message)
{
  emberSerialPrintf(APP_SERIAL, "RX type %x from %2x, cluster %2x, len %x",
                    type, emberGetSender(),
                    apsFrame->clusterId, emberMessageBufferLength(message));
}

// end Ember callback handlers
// *******************************************************************

// *****************************
// msVersionCB
// *****************************
void msVersionCB(void)
{
  emberSerialPrintf(APP_SERIAL,
                    "Manufacturing Library Sample Application, version 1.0\r\n\r\n");
}

// *****************************
// printEUI64 - utility function
// *****************************
void printEUI64(uint8_t port, EmberEUI64* eui)
{
  uint8_t i;
  uint8_t* p = (uint8_t*)eui;

  emberSerialPrintf(port, "EUI: ");
  for (i = 8; i > 0; i--) {
    emberSerialPrintf(port, "%x", p[i - 1]);
  }
  emberSerialPrintf(port, "\r\n");
  emberSerialWaitSend(APP_SERIAL);
}

// *********************************
// printExtendedPanId
// *********************************
void printExtendedPanId(uint8_t port, uint8_t *extendedPanId)
{
  uint8_t i;
  emberSerialPrintf(port, "   ExtendedPanId: ");
  for (i = 0; i < EXTENDED_PAN_ID_SIZE; i++) {
    emberSerialPrintf(port, "%x", extendedPanId[i]);
  }
  emberSerialPrintf(port, "\r\n");
  emberSerialWaitSend(APP_SERIAL);
}

// *****************************
// msInfoCB
// *****************************
// mfg-sample, version x
// stack aabb
// EUI X
// in mfg mode: YES / NO
//    mfg channel: X, mfg power: X
//    tone running / stream running
// joined to network: NO
//    shortID / channel / power / panid / ext pan id
//
void msInfoCB(void)
{
  EmberNetworkParameters parameters;
  EmberStatus status;
  status = emberGetNetworkParameters(&parameters);
  if (status != EMBER_SUCCESS) {
    emberSerialPrintf(APP_SERIAL,
                      "Error: get network parameter failed, status 0x%x\r\n", status);
  }

  msVersionCB();
  printEUI64(APP_SERIAL, (EmberEUI64*)emberGetEui64());
  emberSerialPrintf(APP_SERIAL, "stack [%2x]\r\n",
                    SOFTWARE_VERSION);
  emberSerialPrintf(APP_SERIAL, "in mfg mode: %p\r\n",
                    mfgLibRunning ? "YES" : "NO");
  emberSerialWaitSend(APP_SERIAL);

  // print info on mfg lib if it is running
  if (mfgLibRunning == true) {
    emberSerialPrintf(APP_SERIAL, "   mfg channel: 0x%x, mfg power: 0x%x",
                      mfglibGetChannel(), mfglibGetPower());
    emberSerialPrintf(APP_SERIAL, "   TX power mode: 0x%2x\r\n",
                      emberGetTxPowerMode());
    emberSerialPrintf(APP_SERIAL, "   mfg options: 0x%x\r\n",
                      mfglibGetOptions());
    emberSerialPrintf(APP_SERIAL, "   mfg pkts received: 0x%2x\r\n",
                      mfgTotalPacketCounter);
    if (mfgToneTestRunning) {
      emberSerialPrintf(APP_SERIAL, "   mfg Tone test running\r\n");
    }
    if (mfgStreamTestRunning) {
      emberSerialPrintf(APP_SERIAL, "   mfg Stream test running\r\n");
    }
  }

  emberSerialPrintf(APP_SERIAL, "stack is up: %p\r\n",
                    stackUp ? "YES" : "NO");
  emberSerialWaitSend(APP_SERIAL);

  // print info on stack if it is up
  if (stackUp == true) {
    emberSerialPrintf(APP_SERIAL, "   short ID [%2x]\r\n", emberGetNodeId());
    emberSerialWaitSend(APP_SERIAL);
    emberSerialPrintf(APP_SERIAL,
                      "   chan [0x%x], power [0x%x], panId [0x%2x]\r\n",
                      parameters.radioChannel,
                      parameters.radioTxPower,
                      parameters.panId);
    printExtendedPanId(APP_SERIAL, parameters.extendedPanId);
    emberSerialWaitSend(APP_SERIAL);
  }
  emberSerialPrintf(APP_SERIAL, "\r\n");
  emberSerialWaitSend(APP_SERIAL);
}

// *****************************
// msNetwork CB
// *****************************
// network form <channel> <power> <panid in hex>
// network join <channel> <power> <panid in hex>
// network leave
// network pjoin <time>
//
// devices always join as routers
// pjoin stands for permitjoining
void msNetworkCB(void)
{
  EmberStatus status;
  uint8_t nodeType;
  uint8_t permitJoinDuration;
  EmberNetworkParameters networkParams;

  // network form <channel> <power> <panid in hex>
  if (cliCompareStringToArgument("form", 1) == true) {
    networkParams.radioChannel = cliGetInt16uFromArgument(2);
    networkParams.radioTxPower = cliGetInt16uFromArgument(3);
    networkParams.panId = (cliGetHexByteFromArgument(0, 4) * 256)
                          + (cliGetHexByteFromArgument(1, 4));

    status = emberFormNetwork(&networkParams);
    emberSerialPrintf(APP_SERIAL, "form 0x%x\r\n\r\n", status);
    return;
  }
  // network join <channel> <power> <panid in hex>
  else if (cliCompareStringToArgument("join", 1) == true) {
    nodeType = EMBER_ROUTER;
    MEMSET(&networkParams, 0, sizeof(EmberNetworkParameters));
    MEMSET(networkParams.extendedPanId,
           0,
           EXTENDED_PAN_ID_SIZE);
    networkParams.radioChannel = cliGetInt16uFromArgument(2);
    networkParams.radioTxPower = cliGetInt16uFromArgument(3);
    networkParams.panId = (cliGetHexByteFromArgument(0, 4) * 256)
                          + (cliGetHexByteFromArgument(1, 4));
    networkParams.joinMethod = EMBER_USE_MAC_ASSOCIATION;

    status = emberJoinNetwork(nodeType, &networkParams);
    emberSerialPrintf(APP_SERIAL, "join 0x%x\r\n\r\n", status);
    return;
  }
  // network init
  else if (cliCompareStringToArgument("init", 1) == true) {
    status = emberNetworkInit(NULL);
    emberSerialPrintf(APP_SERIAL, "init 0x%x\r\n\r\n", status);
    return;
  }
  // network leave
  else if (cliCompareStringToArgument("leave", 1) == true) {
    status = emberLeaveNetwork();
    emberSerialPrintf(APP_SERIAL, "leave 0x%x\r\n\r\n", status);
    return;
  }
  // network pjoin <time>
  else if (cliCompareStringToArgument("pjoin", 1) == true) {
    permitJoinDuration = cliGetInt16uFromArgument(2);
    emberSerialPrintf(APP_SERIAL, "val 0x%x\r\n", permitJoinDuration);
    status = emberPermitJoining(permitJoinDuration);
    emberSerialPrintf(APP_SERIAL, "pJoin 0x%x\r\n\r\n", status);
    return;
  } else {
    emberSerialPrintf(APP_SERIAL,
                      "network form <channel> <power> <panid in hex>\r\n");
    emberSerialPrintf(APP_SERIAL,
                      "network join <channel> <power> <panid in hex>\r\n");
    emberSerialPrintf(APP_SERIAL, "network init\r\n");
    emberSerialWaitSend(APP_SERIAL);
    emberSerialPrintf(APP_SERIAL, "network leave\r\n");
    emberSerialPrintf(APP_SERIAL, "network pjoin <time>\r\n");
    emberSerialPrintf(APP_SERIAL, "\r\n");
  }
}

#define MFGAPP_TEST_PACKET_MAX_SIZE 0x40
uint8_t testPacket[] = {
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
  0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
  0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
  0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
  0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
  0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
  0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40
};

// utility used by msMfgCB
void fillBuffer(uint8_t* buff, uint8_t length, bool random)
{
  uint8_t i;
  // length byte does not include itself. If the user asks for 10
  // bytes of packet this means 1 byte length, 7 bytes, and 2 bytes CRC
  // this example will have a length byte of 9, but 10 bytes will show
  // up on the receive side
  buff[0] = length - 1;

  if (random) {
    for (i = 1; i < length; i++) {
      buff[i] = halCommonGetRandom();
    }
  } else {
    for (i = 1; i < length; i++) {
      buff[i] = testPacket[i - 1];
    }
  }
}

// *****************************
// msMfgCB
// *****************************
// mfg start <want callback, 0=False, 1=True>
// mfg end
// mfg tone start
// mfg tone stop
// mfg stream start
// mfg stream stop
// mfg send <numPackets> test-packet <length>
// mfg send <numPackets> random <length>
// mfg send <numPackets> message <first 8 bytes> <second 8 bytes>
// mfg chan set <channel>
// mfg chan get
// mfg power set <power> mode <powermode>
// mfg power get
// mfg testmodcal <channel> <ms>
void msMfgCB(void)
{
  EmberStatus status;

  // mfg start <callback>
  if (cliCompareStringToArgument("start", 1) == true) {
    uint8_t callback = cliGetInt16uFromArgument(2);
    if (callback == 1) {
      // Pass in mfglibRxHandler()
      status = mfglibStart((*mfglibRxHandler));
    } else {
      status = mfglibStart(NULL);
    }
    emberSerialPrintf(APP_SERIAL, "mfglib start (%x), status 0x%x\r\n\r\n",
                      callback, status);

    // set a flag indicating to the app that the mfg lib is running
    if (status == EMBER_SUCCESS) {
      mfgLibRunning = true;
      mfgTotalPacketCounter = 0;
    }
  }
  // mfg end
  else if (cliCompareStringToArgument("end", 1) == true) {
    emberSerialPrintf(APP_SERIAL, "rx 0x%2x packets while in mfg mode\r\n",
                      mfgTotalPacketCounter);
    status = mfglibEnd();
    emberSerialPrintf(APP_SERIAL, "mfglib end status 0x%x\r\n\r\n", status);

    // set a flag indicating to the app that the mfg lib is not running
    if (status == EMBER_SUCCESS) {
      mfgLibRunning = false;
    }
  }
  // mfg tone
  else if (cliCompareStringToArgument("tone", 1) == true) {
    if (cliCompareStringToArgument("start", 2) == true) {
      status = mfglibStartTone();
      emberSerialPrintf(APP_SERIAL, "start tone 0x%x\r\n\r\n", status);
      if (status == EMBER_SUCCESS) {
        mfgToneTestRunning = true;
      }
    } else {
      status = mfglibStopTone();
      emberSerialPrintf(APP_SERIAL, "stop tone 0x%x\r\n\r\n", status);
      if (status == EMBER_SUCCESS) {
        mfgToneTestRunning = false;
      }
    }
  }
  // mfg stream
  else if (cliCompareStringToArgument("stream", 1) == true) {
    if (cliCompareStringToArgument("start", 2) == true) {
      status = mfglibStartStream();
      emberSerialPrintf(APP_SERIAL, "start stream 0x%x\r\n\r\n", status);
      if (status == EMBER_SUCCESS) {
        mfgStreamTestRunning = true;
      }
    } else {
      status = mfglibStopStream();
      emberSerialPrintf(APP_SERIAL, "stop stream 0x%x\r\n\r\n", status);
      if (status == EMBER_SUCCESS) {
        mfgStreamTestRunning = false;
      }
    }
  }
  //  0   1       2          3            4
  // mfg send <numPackets> random       <length>
  // mfg send <numPackets> test-packet  <length>
  //                       message      <up to 8 bytes>
  else if (cliCompareStringToArgument("send", 1) == true) {
    uint16_t i;
    uint16_t numPackets = cliGetInt16uFromArgument(2);
    uint16_t length = cliGetInt16uFromArgument(4);

    // handle the case where the data is random
    if (cliCompareStringToArgument("random", 3) == true) {
      fillBuffer((uint8_t*) &sendBuff, length, true);
    }
    // handle the case where the data is the test packet
    else if (cliCompareStringToArgument("test", 3) == true) {
      // error check the length, if you hit this just increase
      // the size and definition of testPacket above.
      if (length > MFGAPP_TEST_PACKET_MAX_SIZE) {
        emberSerialPrintf(APP_SERIAL,
                          "length of 0x%x larger than current test packet (0x%x)\r\n",
                          length, MFGAPP_TEST_PACKET_MAX_SIZE);
        return;
      }
      fillBuffer((uint8_t*) &sendBuff, length, false);
    }
    // handle the case where the data is specified by the user
    else if (cliCompareStringToArgument("message", 3) == true) {
      for (i = 0; i < 8; i++) {
        sendBuff[i] = cliGetHexByteFromArgument(i, 4);
        sendBuff[i + 8] = cliGetHexByteFromArgument(i, 5);
        sendBuff[i + 16] = cliGetHexByteFromArgument(i, 6);
        sendBuff[i + 24] = cliGetHexByteFromArgument(i, 7);
      }
      // length byte does not include itself, so if the length byte
      // says 7, this means 7 bytes of data and the length byte = 8 total
      length = sendBuff[0] + 1;
    }
    // handle the case where the input is unrecognized - error
    else {
      emberSerialPrintf(APP_SERIAL, "not recognized, try \"mfg\" for help\r\n");
      return;
    }

    // check the length for min and max
    // in this case, "length" counts the 15.4 length byte.
    if (length > 128) {
      emberSerialPrintf(APP_SERIAL,
                        "adjusting length from 0x%x to maximum of 128\r\n");
      length = 128;
      sendBuff[0] = 127;
    }
    if (length < 6) {
      emberSerialPrintf(APP_SERIAL,
                        "adjusting length from 0x%x to minimum of 6\r\n");
      length = 6;
      sendBuff[0] = 5;
    }

    // debug message describing packet that will be sent
    emberSerialPrintf(APP_SERIAL,
                      "sending mfglib 0x%2x packet(s), length 0x%x",
                      numPackets, length);

    for (i = 0; i < length; i++) {
      if ((i % 16) == 0) {
        emberSerialPrintf(APP_SERIAL, "\r\n   : ");
      }
      emberSerialPrintf(APP_SERIAL, " %x", sendBuff[i]);
      emberSerialWaitSend(APP_SERIAL);
    }
    emberSerialPrintf(APP_SERIAL, "\r\n");

    // send the packet(s)

    if (numPackets == 0) {
      emberSerialPrintfLine(APP_SERIAL,
                            "Error: number of packets must be > 0");
    } else {
      // The second parameter to the mfglibSendPacket() is the
      // number of "repeats", therefore we decrement numPackets by 1.
      numPackets--;
      status = mfglibSendPacket(sendBuff, numPackets);

      // print an error on failure
      if (status != EMBER_SUCCESS) {
        emberSerialPrintf(APP_SERIAL,
                          "mfg send err 0x%x \r\n\r\n", status);
      }
    }
  }
  // mfg chan
  else if (cliCompareStringToArgument("chan", 1) == true) {
    if (cliCompareStringToArgument("get", 2) == true) {
      emberSerialPrintf(APP_SERIAL, "mfg get channel 0x%x\r\n\r\n",
                        mfglibGetChannel());
    } else if (cliCompareStringToArgument("set", 2) == true) {
      uint16_t channel = cliGetInt16uFromArgument(3);
      status = mfglibSetChannel(LOW_BYTE(channel));
      emberSerialPrintf(APP_SERIAL,
                        "mfg set channel to 0x%x, status 0x%x\r\n\r\n",
                        LOW_BYTE(channel), status);
    }
  }
  // mfg power
  else if (cliCompareStringToArgument("power", 1) == true) {
    if (cliCompareStringToArgument("get", 2) == true) {
      emberSerialPrintf(APP_SERIAL, "mfg get power 0x%x (mode %2x)\r\n\r\n",
                        mfglibGetPower(), emberGetTxPowerMode());
    } else if (cliCompareStringToArgument("set", 2) == true) {
      if (cliCompareStringToArgument("mode", 4) == true) {
        uint16_t power = cliGetInt16uFromArgument(3);
        uint16_t powermode = cliGetInt16uFromArgument(5);
        // can use EMBER_TX_POWER_MODE_DEFAULT = 0
        //         EMBER_TX_POWER_MODE_BOOST = 1
        //         EMBER_TX_POWER_MODE_ALTERNATE = 2
        if (powermode <= EMBER_TX_POWER_MODE_ALTERNATE) {
          status = mfglibSetPower(powermode, LOW_BYTE(power));
          emberSerialPrintf(APP_SERIAL,
                            "mfg set power to 0x%x, mode 0x%2x, status 0x%x\r\n\r\n",
                            LOW_BYTE(power), powermode, status);
        }
      }
    }
  }
  // mfg options
  else if (cliCompareStringToArgument("options", 1) == true) {
    if (cliCompareStringToArgument("get", 2) == true) {
      emberSerialPrintf(APP_SERIAL, "mfg get options 0x%x\r\n\r\n",
                        mfglibGetOptions());
    } else if (cliCompareStringToArgument("set", 2) == true) {
      uint8_t options = cliGetInt16uFromArgument(3);
      status = mfglibSetOptions(options);
      emberSerialPrintf(APP_SERIAL,
                        "mfg set options to 0x%x, status 0x%x\r\n\r\n",
                        options, status);
    }
  } else if (cliCompareStringToArgument("testmodcal", 1) == true) {
    uint8_t channel = LOW_BYTE(cliGetInt16uFromArgument(2));
    uint32_t duration = cliGetInt32uFromArgument(3);

    emberSerialPrintf(APP_SERIAL, "Setting channel and performing calibration for channel 0x%x\r\n", channel);
    if (duration == 0) {
      emberSerialPrintf(APP_SERIAL, "Continuous Mod DAC Cal, this will never end, reset part to stop.\r\n");
    } else {
      emberSerialPrintf(APP_SERIAL, "Mod DAC Cal running for %u milliseconds\r\n", duration);
    }

    mfglibTestContModCal(channel, duration);
  } else if (cliCompareStringToArgument("synoffset", 1)) {
    if (cliCompareStringToArgument("get", 2)) {
      emberSerialPrintf(APP_SERIAL, "mfg get synth offset: %d steps\r\n\r\n",
                        mfglibGetSynOffset());
    } else if (cliCompareStringToArgument("set", 2)) {
      int8_t synOffset = LOW_BYTE(cliGetInt16sFromArgument(3));
      bool toneWasRunning = mfgToneTestRunning;
      emberSerialPrintf(APP_SERIAL, "mfg set synth offset: %d steps\r\n\r\n",
                        synOffset);

      if (toneWasRunning) {
        mfglibStopTone();
      }

      mfglibSetSynOffset(synOffset);

      if (toneWasRunning) {
        mfglibStartTone();
      }
    }
  } else if (cliCompareStringToArgument("token", 1)) {
    if (cliCompareStringToArgument("read", 2)
        && cliCompareStringToArgument("synoffset", 3)) {
      uint16_t value;
      halCommonGetMfgToken(&value, TOKEN_MFG_SYNTH_FREQ_OFFSET);
      emberSerialPrintf(APP_SERIAL,
                        "mfg read token TOKEN_MFG_SYNTH_FREQ_OFFSET: 0x%2x\r\n\r\n",
                        value);
    } else if (cliCompareStringToArgument("write", 2)
               && cliCompareStringToArgument("synoffset", 3)) {
      // compute the value of the token.  the LSB of the high byte must be 0 in
      // order for the radio to use the token.  the other bits of the high byte
      // are reserved, so leave them unwritten (which in flash is a 1), thus
      // the high byte should be 0xFE
      // NOTE: writing the token can only be done once with this API!  you'll
      // need to use em3xx_load --patch to reset it (or write it) if you want to
      // write it again
      uint16_t value = ((uint16_t)mfglibGetSynOffset() & 0x00FF) | 0xFE00;
      halCommonSetMfgToken(TOKEN_MFG_SYNTH_FREQ_OFFSET, &value);
      emberSerialPrintf(APP_SERIAL,
                        "mfg write token TOKEN_MFG_SYNTH_FREQ_OFFSET: 0x%2x\r\n\r\n",
                        value);
    }
  }
  // error
  else {
    emberSerialPrintf(APP_SERIAL,
                      "mfg start <want callback, 0=False, 1=True>\r\n");
    emberSerialPrintf(APP_SERIAL, "mfg end\r\n");
    emberSerialPrintf(APP_SERIAL, "mfg tone start\r\n");
    emberSerialWaitSend(APP_SERIAL);
    emberSerialPrintf(APP_SERIAL, "mfg tone stop\r\n");
    emberSerialPrintf(APP_SERIAL, "mfg stream start\r\n");
    emberSerialPrintf(APP_SERIAL, "mfg stream stop\r\n");
    emberSerialWaitSend(APP_SERIAL);
    emberSerialPrintf(APP_SERIAL,
                      "mfg send <numPackets> test-packet <length>\r\n");
    emberSerialPrintf(APP_SERIAL, "mfg send <numPackets> random <length>\r\n");
    emberSerialPrintf(APP_SERIAL, "mfg send <numPackets> message ");
    emberSerialWaitSend(APP_SERIAL);
    emberSerialPrintf(APP_SERIAL, "<first 8 bytes> <second 8 bytes>\r\n");
    emberSerialWaitSend(APP_SERIAL);
    emberSerialPrintf(APP_SERIAL, "mfg power set <power> mode <powermode>\r\n");
    emberSerialPrintf(APP_SERIAL, "mfg power get\r\n");
    emberSerialWaitSend(APP_SERIAL);
    emberSerialPrintf(APP_SERIAL, "mfg channel set <channel 11-26>\r\n");
    emberSerialWaitSend(APP_SERIAL);
    emberSerialPrintf(APP_SERIAL, "mfg channel get\r\n");
    emberSerialWaitSend(APP_SERIAL);
    emberSerialPrintf(APP_SERIAL, "mfg options set <options>\r\n");
    emberSerialPrintf(APP_SERIAL, "mfg options get\r\n");
    emberSerialWaitSend(APP_SERIAL);
    emberSerialPrintf(APP_SERIAL, "mfg testmodcal <channel 11-26> <# milliseconds to run, 0 == infinite>\r\n");
    emberSerialWaitSend(APP_SERIAL);
    emberSerialPrintf(APP_SERIAL, "mfg synoffset set <# 11.7kHz steps> - value is only set until next reset\r\n");
    emberSerialPrintf(APP_SERIAL, "mfg synoffset get\r\n");
    emberSerialWaitSend(APP_SERIAL);
    emberSerialPrintf(APP_SERIAL, "mfg token read synoffset\r\n");
    emberSerialPrintf(APP_SERIAL, "mfg token write synoffset\r\n");
    emberSerialWaitSend(APP_SERIAL);
    emberSerialPrintf(APP_SERIAL, "\r\n");
  }
}

// *****************************
// msResetCB
// *****************************
void msResetCB(void)
{
  halReboot();
}

void msHelpCB(void);

// *****************************
// required setup for command line interface (CLI)
// see app/util/serial/cli.h for more information
// *****************************
cliSerialCmdEntry cliCmdList[] = {
  { "help", msHelpCB },
  { "version", msVersionCB },
  { "info", msInfoCB },
  { "mfg", msMfgCB },
  { "network", msNetworkCB },
  { "reset", msResetCB }
};
uint8_t cliCmdListLen = sizeof(cliCmdList) / sizeof(cliSerialCmdEntry);
PGM_P cliPrompt = "mfg-sample";

// *****************************
// msHelpCB
// *****************************
void msHelpCB()
{
  uint8_t i;
  for (i = 0; i < cliCmdListLen; i++) {
    emberSerialPrintf(APP_SERIAL, "%p\r\n", cliCmdList[i].cmd);
  }
  emberSerialPrintf(APP_SERIAL, "\r\n");
}
