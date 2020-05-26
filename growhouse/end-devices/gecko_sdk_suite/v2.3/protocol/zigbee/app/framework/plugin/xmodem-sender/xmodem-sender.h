#define XMODEM_SOH   (0x01)
#define XMODEM_EOT   (0x04)
#define XMODEM_ACK   (0x06)
#define XMODEM_NAK   (0x15)
#define XMODEM_CANCEL (0x18)
#define XMODEM_BLOCKOK (0x19)
#define XMODEM_FILEDONE (0x17)

typedef EmberStatus (EmberAfXmodemSenderTransmitFunction)(uint8_t* data, uint8_t length);

typedef EmberStatus (EmberAfXmodemSenderGetNextBlockFunction)(uint32_t address,
                                                              uint8_t length,
                                                              uint8_t* returnData,
                                                              uint8_t* returnLength,
                                                              bool* done);
typedef void (EmberAfXmodemSenderFinishedFunction)(bool success);

void emberAfPluginXmodemSenderIncomingBlock(uint8_t* data,
                                            uint8_t  length);

// The maxSizeOfBlock does not include the Xmodem overhead (5-bytes)
EmberStatus emberAfPluginXmodemSenderStart(EmberAfXmodemSenderTransmitFunction* sendRoutine,
                                           EmberAfXmodemSenderGetNextBlockFunction* getNextBlockRoutine,
                                           EmberAfXmodemSenderFinishedFunction* finishedRoutine,
                                           uint8_t maxSizeOfBlock,
                                           bool waitForReady);

void emberAfPluginXmodemSenderAbort(void);
