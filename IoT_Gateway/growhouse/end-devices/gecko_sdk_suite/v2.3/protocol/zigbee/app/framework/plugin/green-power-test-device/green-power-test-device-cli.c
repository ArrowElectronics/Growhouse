#include "app/framework/include/af.h"
//#include "stack/gp/gp-proxy-table.h"
#include "phy/phy.h"
#include "stack/core/ember-multi-network.h"
//#include "green-power-client.h"
#include "stack/platform/micro/aes.h"

#ifndef EMBER_AF_GENERATE_CLI
#error The Green Power Test Device plugin is not compatible with the legacy CLI.
#endif

void emCalculateMic(EmberMessageBuffer header,
                    uint8_t *contents,
                    uint8_t authenticationStartIndex,
                    uint8_t encryptionStartIndex,
                    uint8_t packetLength,
                    bool useCbcMac,
                    uint8_t nonce[],
                    uint8_t micResult[]);

void emEncryptBytes(uint8_t* bytes,
                    uint8_t length,
                    uint16_t blockCount,
                    uint8_t nonce[]);

static uint32_t gpdSFC = 1;
static uint32_t srcId = 0x0050007F;
EmberEUI64 gpdIeee;

#define STANDALONE_FLAGS_INDEX                   0
#define STANDALONE_NONCE_SOURCE_ADDR_INDEX       1
#define STANDALONE_NONCE_FRAME_COUNTER_INDEX     9
#define STANDALONE_NONCE_SECURITY_CONTROL_INDEX 13
#define STANDALONE_VARIABLE_FIELD_INDEX_HIGH    14
#define STANDALONE_VARIABLE_FIELD_INDEX_LOW     15

uint8_t key[16] =  {
  0xb9,
  0xfb,
  0xce,
  0xf7,
  0x29,
  0x09,
  0x76,
  0x14,
  0x19,
  0x2a,
  0x01,
  0x5a,
  0x96,
  0x41,
  0xfc,
  0x50
};

static void initializeNonce(uint8_t *nonce)
{
  nonce[0] = 0x49;
  MEMMOVE(nonce + 1, &(srcId), 4);
  MEMMOVE(nonce + 5, &(srcId), 4);

  // Frame counter.
  nonce[STANDALONE_NONCE_FRAME_COUNTER_INDEX] = BYTE_0(gpdSFC);
  nonce[STANDALONE_NONCE_FRAME_COUNTER_INDEX + 1] = BYTE_1(gpdSFC);
  nonce[STANDALONE_NONCE_FRAME_COUNTER_INDEX + 2] = BYTE_2(gpdSFC);
  nonce[STANDALONE_NONCE_FRAME_COUNTER_INDEX + 3] = BYTE_3(gpdSFC);

  // Security level.
  nonce[STANDALONE_NONCE_SECURITY_CONTROL_INDEX] = 0x05; //this is for incoming which is the only thing we use, note that appid 2 outgoing is 0xC5
}

#define encryptMic(mic, nonce) \
  (emEncryptBytes((mic), 4, 0, (nonce)))

void emberAfPluginGreenPowerTestDeviceSendTestIeee(void)
{
  uint8_t testMessage2[] =
  {
    0x41, 0xC8, 0x5A,//mac framecontrol + sequence
    0xFF, 0xFF, 0xFF, 0xFF, //dest and pan
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, //fill in Ieee here
    0x8C, 0x02,  //stub nwk fcf
    0x00,   //endpoint
    0x13,
  };
  EmberMessageBuffer mesg = emberFillLinkedBuffers(testMessage2, sizeof(testMessage2));
  if (mesg == EMBER_NULL_MESSAGE_BUFFER) {
    return;
  }

  emberSendRawMessage(mesg);
  emberReleaseMessageBuffer(mesg);
}

void emberAfPluginGreenPowerTestDeviceSendTest(void)
{
  uint8_t beginning[] = {
    0x01,
    0x08,
    0x83, //sequence
    0xFF, //destination
    0xFF,
    0xFF,
    0xFF,
    0x8C,//nwkFCF
    0x30,//extnwkFCF
  };

  uint8_t bytes[4];
  uint8_t nonce[16];
  uint8_t mic[4];
  uint8_t gpdCommandPayload[55];

  uint8_t gpdCommandId = emberUnsignedCommandArgument(0);
  uint8_t gpdCommandPayloadLen = emberCopyStringArgument(1, gpdCommandPayload, 55, false);

  EmberMessageBuffer mesg = emberFillLinkedBuffers(beginning, sizeof(beginning));
  if (mesg == EMBER_NULL_MESSAGE_BUFFER) {
    return;
  }

  emberStoreLowHighInt32u(bytes, srcId);
  emberAppendToLinkedBuffers(mesg, bytes, 4);

  emberStoreLowHighInt32u(bytes, gpdSFC);
  emberAppendToLinkedBuffers(mesg, bytes, 4);

  emberAppendToLinkedBuffers(mesg, &gpdCommandId, 1);

  emberAppendToLinkedBuffers(mesg, gpdCommandPayload, gpdCommandPayloadLen);

  emLoadKeyIntoCore(key);

  initializeNonce(nonce);

  emCalculateMic(mesg,
                 NULL,
                 7,//auth start index
                 emberMessageBufferLength(mesg), // encryption start index
                 emberMessageBufferLength(mesg), // length
                 false, //not cbc mic
                 nonce,
                 mic

                 );
  encryptMic(mic, nonce);

  emberAfGreenPowerClusterPrintln("mic is %x %x %x %x", mic[0], mic[1], mic[2], mic[3]);

  emberAppendToLinkedBuffers(mesg, mic, 4);

  emberSendRawMessage(mesg);
  emberReleaseMessageBuffer(mesg);
  gpdSFC++;
}

void emberAfPluginGreenPowerTestDeviceSetupGpd(void)
{
  srcId = emberUnsignedCommandArgument(0);
  emberCopyStringArgument(1, key, 16, true);
}

void emberAfPluginGreenPowerTestDeviceSetupGpdIeee(void)
{
  emberCopyStringArgument(0, gpdIeee, 8, true);
  emberCopyStringArgument(1, key, 16, true);
}

// plugin green-power-test-device send-comm <type:1>
void emberAfPluginGreenPowerTestDeviceSendCommission(void)
{
  uint8_t testMessage0[] =
  {
    //0x2A,
    0x01, 0x08, 0x5A, 0xFF, 0xFF, 0xFF, 0xFF,// MAC:
    0x0C,// NWK FCF:
    0x7F, 0x00, 0x50, 0x00,// SrcID :
    0xE0, //Command:
    0x02, //GPD device ID:
    0xC1, //options: --incremental mac seqnum, fixed location, ext opts
    0xF2, //extopt: seclevel2, keytype0b100, keypresent, keycrypt,outcount present
    0x41, 0x9B, 0x14, 0xDF, 0xD2, 0xA7, 0x11, 0x08, 0xD0, 0x31, 0x04, 0x62, 0x76, 0x57, 0xF5, 0x05,// Key:
    0x09, 0x6A, 0x66, 0x32,// keymic:
    0x01, 0x00, 0x00, 0x00,// gpdoutcount:
    //radiostuff: D0 42 00 7B 00               ..B.{.
  };
  /*
     uint8_t testMessage1[] =
     {
     //0x2A,
     0x01,0x08,0x5A,0xFF,0xFF,0xFF,0xFF,// MAC:
     0x0C,// NWK FCF:
     0x21,0x43,0x65,0x87,// SrcID :
     0xE0 ,//Command:
     0x02 ,//GPD device ID:
     0xC3 ,//options: --incremental mac seqnum, fixed location, ext opts
     0x9E ,//extopt: seclevel2, keytype0b111, outcount present
     0x01,0x00,0x00,0x00,// gpdoutcount:
     };
   */

  uint8_t testMessage2[] =
  {
    0x41, 0xC8, 0x5A,//mac framecontrol + sequence
    0xFF, 0xFF, 0xFF, 0xFF, //dest and pan
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, //fill in Ieee here
    0x8C, 0x02,  //stub nwk fcf
    0x00,   //endpoint
    0xE0,   //command
    0x02,   //gpd device Id
    0x41,   //options - incremental seqnum, fixed location
  };

  EmberMessageBuffer mesg;
  uint8_t selection = emberUnsignedCommandArgument(0);
  //commissioning message from switch

  if (selection == 0) {
    mesg = emberFillLinkedBuffers(testMessage0, 41);
  } else if (selection == 1) {
    emberStoreLowHighInt32u(testMessage0 + 8, srcId);
    testMessage0[15] &= ~0x40; //turn off key encryption
    MEMMOVE(testMessage0 + 16, key, 16);
    mesg = emberFillLinkedBuffers(testMessage0, 41);
  } else if (selection == 2) {
    mesg = emberFillLinkedBuffers(testMessage2, sizeof(testMessage2));
  } else {
    return;
  }
  if (mesg == EMBER_NULL_MESSAGE_BUFFER) {
    return;
  }

  emberSendRawMessage(mesg);
  emberReleaseMessageBuffer(mesg);
}
static uint8_t rxChannel;

// plugin green-power-test-device send-channel-request <channel:1>
void emberAfPluginGreenPowerTestDeviceSendChannelRequest(void)
{
  uint8_t channel = emberUnsignedCommandArgument(0);
  uint8_t testMessage[] =
  {
    0x01, 0x08, 0x5A, 0xFF, 0xFF, 0xFF, 0xFF,// MAC:
    0x0D,  // NWK FCF: data frame
    //0x7F,0x00,0x50,0x00,// SrcID :
    EMBER_ZCL_GP_GPDF_CHANNEL_REQUEST,
    //0x11// rx channel 12 in next two attempts
    (((rxChannel - 11) << 4) | (rxChannel - 11))
  };
  EmberMessageBuffer mesg = emberFillLinkedBuffers(testMessage, 10);

  if (mesg == EMBER_NULL_MESSAGE_BUFFER) {
    return;
  }

  emberSendRawMessage(mesg);
  emberReleaseMessageBuffer(mesg);
  emSetPhyRadioChannel(channel);
  emStackRadioChannel = channel;
  //only listen on our channel
  if (channel == rxChannel) {
    emRadioSetIdleMode(EMBER_RADIO_POWER_MODE_RX_ON);
  } else {
    emRadioSetIdleMode(EMBER_RADIO_POWER_MODE_OFF);
  }
}

static uint8_t netChannel;
void gpSetNetChannel(uint8_t c)
{
  netChannel = c;
}
void emberAfPluginGreenPowerTestDeviceSetRadioChannel(void)
{
  uint8_t channel = emberUnsignedCommandArgument(0);
  if (channel == 255) {
    emberAfGreenPowerClusterPrintln("setting our channel to %d", netChannel);
    channel = netChannel;
    emRadioSetIdleMode(EMBER_RADIO_POWER_MODE_RX_ON);
  }
  emSetPhyRadioChannel(channel);
  emStackRadioChannel = channel;
  rxChannel = channel;
}
