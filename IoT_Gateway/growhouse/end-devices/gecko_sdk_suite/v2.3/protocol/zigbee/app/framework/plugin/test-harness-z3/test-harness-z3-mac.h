//
// test-harness-z3-mac.h
//
// ZigBee 3.0 zdo test harness functionality
//

#define BEACON_REQUEST_COMMAND        (0x07)
#define BEACON_ASSOCIATION_PERMIT_BIT (0x80)
#define BEACON_EXTENDED_PAN_ID_INDEX  (7)

// Zigbee part of MAC Payload, AKA the 'Beacon Payload'
#define BEACON_PAYLOAD_SIZE           (15)

// The size of a 802.15.4 MAC beacon payload in bytes.
#define MAC_BEACON_SIZE (4)

void emAfPluginTestHarnessZ3ModifyBeaconPayload(uint8_t *beaconPayload, uint8_t *payloadLength);
