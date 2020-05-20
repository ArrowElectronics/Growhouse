// *****************************************************************************
// * ota-client-page-request.h
// *
// * Zigbee Over-the-air bootload cluster for upgrading firmware and
// * downloading device specific file.
// * This file handles the page request feature for the client.
// *
// * Copyright 2010 by Ember Corporation. All rights reserved.              *80*
// *****************************************************************************

enum {
  EM_AF_NO_PAGE_REQUEST              = 0,
  EM_AF_WAITING_PAGE_REQUEST_REPLIES = 1,
  EM_AF_RETRY_MISSED_PACKETS         = 2,
  EM_AF_PAGE_REQUEST_COMPLETE        = 3,
  EM_AF_BLOCK_ALREADY_RECEIVED       = 4,
  EM_AF_PAGE_REQUEST_ERROR           = 0xFF
};
typedef uint8_t EmAfPageRequestClientStatus;

#define EM_AF_PAGE_REQUEST_BLOCK_SIZE 32

// This routine returns a timer indicating how long we should wait for
// the page request responses to come in.  0 if there was an error.
uint32_t emAfInitPageRequestClient(uint32_t offsetForPageRequest,
                                   uint32_t totalImageSize);
void emAfPageRequestTimerExpired(void);
bool emAfHandlingPageRequestClient(void);
EmAfPageRequestClientStatus emAfGetCurrentPageRequestStatus(void);
EmAfPageRequestClientStatus emAfNoteReceivedBlockForPageRequestClient(uint32_t offset);
EmAfPageRequestClientStatus emAfNextMissedBlockRequestOffset(uint32_t* nextOffset);

uint32_t emAfGetPageRequestMissedPacketDelayMs(void);
uint32_t emAfGetFinishedPageRequestOffset(void);
void emAfAbortPageRequest(void);
