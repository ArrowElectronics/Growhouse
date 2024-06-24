/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "qdiag_so_client.h"
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/select.h>
#include "hdlc.h"
#include "mbuf.h"
#include "libtcmd.h"
#include "wlan_nv.h"
#include "wlan_nv2.h"
#include "wlan_nv_parser.h"

/*=====================================================================
 Debugging utils
=====================================================================*/

#define FTM_DBG_ERROR   0x00000001
#define FTM_DBG_INFO    0x00000002
#define FTM_DBG_TRACE   0x00000004
#define FTM_DBG_DUMP    0x00000008

#define FTM_DBG_DEFAULT FTM_DBG_DUMP //(FTM_DBG_ERROR | FTM_DBG_INFO | FTM_DBG_TRACE)

unsigned int g_dbg_level = FTM_DBG_DEFAULT;

#define DPRINTF(_level, _x...)\
    do {\
        if (g_dbg_level & (_level))\
        {\
            fprintf(stderr, ##_x);\
        }\
    } while (0);

#define DUMP_CAP 2048

void print_uchar_array(uint8_t *addr, size_t len)
{
   size_t i;
   DPRINTF(FTM_DBG_DUMP, "HEXDUMP\n");
   for (i = 0; i < len && i < DUMP_CAP; i++) {
	  if (i % 16 == 0) {
		  DPRINTF(FTM_DBG_DUMP, "0x%08X: ", i);
	  }
      DPRINTF(FTM_DBG_DUMP, "%02X ", addr[i]);
      if (i % 16 == 15) {
    	  DPRINTF(FTM_DBG_DUMP, "\n");
      }
   }
   DPRINTF(FTM_DBG_DUMP, "\nHEXDUMP\n");
}

void print_uint16_array(uint16_t *addr, size_t len)
{
   size_t i;
   for (i = 0; i < len && i < DUMP_CAP; i++) {
	  if (i % 8 == 0) {
		  DPRINTF(FTM_DBG_DUMP, "0x%08X: ", i);
	  }
      DPRINTF(FTM_DBG_DUMP, "%02X %02X|", addr[i]>>8, addr[i]&0xFF);
	  if (i % 8 == 7) {
		  DPRINTF(FTM_DBG_DUMP, "\n");
	  }
   }
   DPRINTF(FTM_DBG_DUMP, "\n");
}

/*=====================================================================
 WLAN Interface info
=====================================================================*/

char g_ifname[IFNAMSIZ] = "wlan";
bool ifs_init[32];

/*=====================================================================
 DIAG FTM subsystem command
=====================================================================*/

int qdiag_handle_ftm_wlan(void *dc,void * data);
int qdiag_fd=0;

typedef uint8_t diagpkt_cmd_code_type;
#define DIAG_SUBSYS_CMD 75 // 0x4B

typedef uint8_t diagpkt_subsys_id_type;
#define DIAG_SUBSYS_FTM 11 // 0x0B

typedef uint16_t diagpkt_subsys_cmd_code_type;
#define FTM_WLAN_CMD_CODE 22 // 0x0016

struct diagpkt_subsys_header_type
{
  diagpkt_cmd_code_type              cmd_code;
  diagpkt_subsys_id_type             subsys_id;
  diagpkt_subsys_cmd_code_type       subsys_cmd_code;
} __attribute__((packed)) g_diag_hdr = { DIAG_SUBSYS_CMD, DIAG_SUBSYS_FTM, FTM_WLAN_CMD_CODE };

struct qdiag_table_entry g_entry;

/*===========================================================================*/

int ftm_diag_encode_send(uint8_t *pkt, size_t length)
{
	size_t i, written = 0;
	uint8_t *outbuf = pkt;
	size_t outlen = length;

	DPRINTF(FTM_DBG_TRACE, "Encoding buffer of %lu bytes:\n", length);
	//print_uchar_array(pkt, length);

	outbuf = hdlc_encode(pkt, length, &outlen);

	written = write(qdiag_fd, outbuf, outlen);
	DPRINTF(FTM_DBG_TRACE, "Wrote %lu out of %lu\n", written, outlen);

	return outlen;
}

/*=====================================================================
 FTM_WLAN commands
=====================================================================*/

#define FTM_WLAN_COMMON_OP 0

/* FTM (WLAN) PKT Header */
struct ftm_wlan_cmd_header_type
{
  uint16_t cmd_id;            /* command id (required) typically FTM_WLAN_COMMON_OP */
  uint16_t cmd_data_len;      /* request pkt data length, excluding the diag and ftm headers
                             (optional, set to 0 if not used)*/
  uint16_t cmd_rsp_pkt_size;  /* rsp pkt size, size of response pkt if different then req pkt
                             (optional, set to 0 if not used)*/
} __attribute__((packed));

/* All wcn36xx testmode interface commands specified in
 * WCN36XX_TM_ATTR_CMD
 */
enum wcn36xx_tm_cmd {
	/* Returns the supported wcn36xx testmode interface version in
	 * WCN36XX_TM_ATTR_VERSION. Always guaranteed to work. User space
	 * uses this to verify it's using the correct version of the
	 * testmode interface
	 */
	WCN36XX_TM_CMD_GET_VERSION = 0,

	/* The netdev interface must be down at the
	 * time.
	 */
	WCN36XX_TM_CMD_START = 1,

	/* Puts the driver back into OFF state.
	 */
	WCN36XX_TM_CMD_STOP = 2,

	/* The command used to transmit a PTT command to the firmware.
	 * Command payload is provided in WCN36XX_TM_ATTR_DATA.
	 */
	WCN36XX_TM_CMD_PTT = 3,
};

/* FTM_MSG */
struct ftm_msg
{
	uint16_t   msgId;
	uint16_t   msgBodyLength;
	uint32_t   respStatus;
	uint8_t    msgResponse[0];
};

#define QUALCOMM_MODULE_NUMBER 2

/* FTM WLAN request packet type */
struct ftm_wlan_req_pkt_type
{
  struct diagpkt_subsys_header_type diag_hdr;
  struct ftm_wlan_cmd_header_type   ftm_hdr;
  uint16_t                     		module_type;  // Typically QUALCOMM_MODULE_NUMBER
  uint16_t                     		ftm_cmd_type; // enum wcn36xx_tm_cmd
  uint8_t                      		data[0];      // struct ftm_msg
} __attribute__((packed));

/* Various ERROR CODES supported by the FTM WLAN module*/
enum ftm_wlan_error_codes {
    FTM_ERR_CODE_PASS = 0,
    FTM_ERR_CODE_FAIL,
};

/* FTM WLAM response packet type */
struct ftm_wlan_rsp_pkt_type
{
	struct diagpkt_subsys_header_type diag_hdr; // diag header
	struct ftm_wlan_cmd_header_type   ftm_hdr;
    uint16_t                          result ;  // enum ftm_wlan_error_codes
    uint8_t                           data[0];  // struct ftm_msg
} __attribute__((packed));

struct diagpkt_subsys_header_v2_type
{
	uint8_t opaque_header[12];
} __attribute__((packed));

void *diagpkt_subsys_alloc(unsigned int length)
{
	struct ftm_wlan_rsp_pkt_type *pkt = NULL;
	if(-1 == qdiag_fd)
	{
		return NULL;
	}

	pkt = malloc(length);

	if( pkt != NULL )
	{
		memset(pkt, 0, length);
		pkt->diag_hdr = g_diag_hdr;
	}
	return (void *) pkt;
}

struct ftm_wlan_req_pkt_type *g_req = NULL;
struct ftm_wlan_rsp_pkt_type *g_rsp = NULL;

/*=====================================================================
 NV management
=====================================================================*/

struct nv_data {
	uint32_t	is_valid;
	uint32_t	magic_number;
	uint8_t	data[0];
} __attribute__((packed));

static struct nv_state {
	struct nv_data *nv;
	size_t nvSize;
	enum nv_version_type nvVersion;
	uint8_t	  *dict;
	size_t dictSize;
	eNvTable   processingNVTable;
	uint32_t   targetNVTableSize;
	uint8_t   *targetNVTablePointer;
	uint32_t   processedNVTableSize;
	uint8_t   *tempNVTableBuffer;
} ftm_config;

#define NV_BIN_FILE "/lib/firmware/wlan/prima/WCNSS_qcom_wlan_nv.bin"
#define NV_DICT_FILE "/lib/firmware/wlan/prima/WCNSS_wlan_dictionary.dat"

static sHalNv g_nvContents;

/*============================
FUNCTION	read_nv_files

DESCRIPTION
  Read the NV files from persistence

DEPENDENCIES
  NIL

RETURN VALUE
  Returns 0 if files were successfully read otherwise error code

SIDE EFFECTS
  NONE
============================*/
static int read_nv_files()
{
	FILE *nvf;
	size_t size = 0;
	size_t count = 0;
	int ret = 0;
	sNvFields fields;

	ftm_config.nvSize = 0;
	ftm_config.nvVersion = E_NV_INVALID;
	ftm_config.dictSize = 0;

	ftm_config.processingNVTable = NV_MAX_TABLE;
	ftm_config.targetNVTableSize = 0;
	ftm_config.targetNVTablePointer = NULL;
	ftm_config.processedNVTableSize = 0;
	ftm_config.tempNVTableBuffer = (uint8_t *) malloc(MAX_NV_TABLE_SIZE);

	nvf = fopen(NV_BIN_FILE, "r");
	if (nvf == NULL) {
		DPRINTF(FTM_DBG_ERROR, "Failed to open bin file %s for reading\n", NV_BIN_FILE);
		ret = -1;
		goto err_out;
	}

	if (!fseek(nvf, 0, SEEK_END)) {
		size = ftell(nvf);
		DPRINTF(FTM_DBG_INFO, "size=%lu\n", size);
		rewind(nvf);
	}

	ftm_config.nv = malloc(size);
	if (ftm_config.nv == NULL) {
		DPRINTF(FTM_DBG_ERROR, "Failed to allocate NV buffer %lu\n", size);
		ret = -1;
		goto err_out;
	}
	if ((count = fread(ftm_config.nv, 1, size, nvf)) == 0) {
		DPRINTF(FTM_DBG_ERROR, "Failed to read into NV buffer %lu\n", size);
		ret = -1;
		goto err_out;
	}
	ftm_config.nvSize = size - (sizeof(ftm_config.nv->is_valid) + sizeof(ftm_config.nv->magic_number));
	DPRINTF(FTM_DBG_TRACE, "nvSize=%lu\n", ftm_config.nvSize);
	fclose(nvf);

	if (MAGIC_NUMBER == ftm_config.nv->magic_number) {
		DPRINTF(FTM_DBG_ERROR, "Magic read as 0x%08X\n", ftm_config.nv->magic_number);
		ftm_config.nvVersion = E_NV_V3;
	} else {
		ftm_config.nvVersion = E_NV_V2;
	}

	// Parse content of bin file
	nvParser(ftm_config.nv->data, ftm_config.nvSize, &g_nvContents);

	fields = g_nvContents.fields;
	DPRINTF(FTM_DBG_DUMP, "\n\n\nREAD count=%lu!!!!!\n", count);
	DPRINTF(FTM_DBG_DUMP, "productId=");
    print_uint16_array(&fields.productId, 1);
	DPRINTF(FTM_DBG_DUMP, "productBands=%02X\n", fields.productBands);
    DPRINTF(FTM_DBG_DUMP, "wlanNvRevId=%02X should be 2 WC3660\n", fields.wlanNvRevId); //0: WCN1312, 1: WCN1314, 2: WCN3660
    DPRINTF(FTM_DBG_DUMP, "numOfTxChains=%02X\n", fields.numOfTxChains);
    DPRINTF(FTM_DBG_DUMP, "numOfRxChains=%02X\n", fields.numOfRxChains);
    DPRINTF(FTM_DBG_DUMP, "macAddr=");
    print_uchar_array(fields.macAddr, NV_FIELD_MAC_ADDR_SIZE);
    DPRINTF(FTM_DBG_DUMP, "macAddr2=");
    print_uchar_array(fields.macAddr2, NV_FIELD_MAC_ADDR_SIZE);
    DPRINTF(FTM_DBG_DUMP, "macAddr3=");
    print_uchar_array(fields.macAddr3, NV_FIELD_MAC_ADDR_SIZE);
    DPRINTF(FTM_DBG_DUMP, "macAddr4=");
    print_uchar_array(fields.macAddr4, NV_FIELD_MAC_ADDR_SIZE);
    DPRINTF(FTM_DBG_DUMP, "mfgSN=");
    print_uchar_array(fields.mfgSN, NV_FIELD_MFG_SN_SIZE);
    DPRINTF(FTM_DBG_DUMP, "couplerType=%02X\n", fields.couplerType);
	DPRINTF(FTM_DBG_DUMP, "nvVersion=%02X\n", fields.nvVersion);

	DPRINTF(FTM_DBG_DUMP, "DATA:\n");
	print_uchar_array((uint8_t *)ftm_config.nv, ftm_config.nvSize);

	if (ftm_config.nvVersion == E_NV_V3) {
		nvf = fopen(NV_DICT_FILE, "r");
		if (nvf == NULL) {
			DPRINTF(FTM_DBG_ERROR, "Failed to open bin file %s for reading\n", NV_DICT_FILE);
			ret = -1;
			goto err_out;
		}

		if (!fseek(nvf, 0, SEEK_END)) {
			size = ftell(nvf);
			DPRINTF(FTM_DBG_INFO, "size=%lu\n", size);
			rewind(nvf);
		}

		ftm_config.dict = malloc(size);
		if (ftm_config.dict == NULL) {
			DPRINTF(FTM_DBG_ERROR, "Failed to allocate DICT buffer %lu\n", size);
			ret = -1;
			goto err_out;
		}
		if (!fread(ftm_config.dict, 1, size, nvf)) {
			DPRINTF(FTM_DBG_ERROR, "Failed to read into DICT buffer %lu\n", size);
			ret = -1;
			goto err_out;
		}
		ftm_config.dictSize = size;
		DPRINTF(FTM_DBG_TRACE, "dictSize=%lu\n", ftm_config.dictSize);
	}

err_out:
	if (nvf != NULL) {
		fclose(nvf);
	}

	return ret;
}

/*============================
FUNCTION	write_nv_files

DESCRIPTION
  Write the NV files back to persistence

DEPENDENCIES
  NIL

RETURN VALUE
  Returns 0 if files were successfully written otherwise error code

SIDE EFFECTS
  NONE
============================*/
static int write_nv_files()
{
	FILE *nvf;
	size_t size = 0;
	int ret = 0;
	char *backup = NULL;;

	// Create a backup file
	backup = tmpnam(NULL);
	if (rename(NV_BIN_FILE, backup)) {
		ret = -1;
		goto err_out;
	}

	// Open BV file for write
	nvf = fopen(NV_BIN_FILE, "w");
	if (nvf == NULL) {
		DPRINTF(FTM_DBG_ERROR, "Failed to open bin file %s for writing\n", NV_BIN_FILE);
		ret = -1;
		goto err_out;
	}

	size = ftm_config.nvSize + sizeof(ftm_config.nv->is_valid);
	if (!fwrite(ftm_config.nv, 1, size, nvf)) {
		DPRINTF(FTM_DBG_ERROR, "Failed to write NV buffer %lu\n", size);
		ret = -1;
		goto err_out;
	}

err_out:
	if (backup != NULL) {
		remove(backup);
	}
	if (nvf != NULL) {
		fclose(nvf);
	}

	return ret;
}

/*=====================================================================
 Command processing
=====================================================================*/

/*===========================================================================
FUNCTION   ftm_wlan_tcmd_rx

DESCRIPTION
   Call back handler

DEPENDENCIES
  NIL

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE

===========================================================================*/

void ftm_wlan_tcmd_rx(void *buf, int len)
{
	struct ftm_msg *rsp_msg = (struct ftm_msg *)buf;
	uint16_t tcmd = ((struct ftm_msg *)g_req->data)->msgId;
    uint16_t result = FTM_ERR_CODE_PASS;

    DPRINTF(FTM_DBG_TRACE, "Rx call back received with len %d\n", len);

	if (len > 0) {
		if (rsp_msg->msgId == tcmd) {
			DPRINTF(FTM_DBG_TRACE, "TCMD response for %04X matches request %04X!\n", tcmd, rsp_msg->msgId);
			result = rsp_msg->respStatus;
		} else {
			DPRINTF(FTM_DBG_TRACE, "TCMD response for %04X does NOT match request %04X!\n", tcmd, rsp_msg->msgId);
		}
	}

    /* Build the response to be sent */
    g_rsp = (struct ftm_wlan_rsp_pkt_type*)diagpkt_subsys_alloc(
    		(sizeof(struct ftm_wlan_req_pkt_type) +
			len));

    if (g_rsp == NULL) {
        DPRINTF(FTM_DBG_ERROR, "Failed to allocate diag packet! tcmd: %d",
                tcmd);
        return;
    }

    g_rsp->ftm_hdr = g_req->ftm_hdr;
    g_rsp->ftm_hdr.cmd_rsp_pkt_size = sizeof(struct ftm_wlan_req_pkt_type) + len;
	g_rsp->result = result;

    if (buf && len != 0) {
        DPRINTF(FTM_DBG_INFO, "Copying data to global response\n");
        memcpy(&g_rsp->data, buf, len);
    }
}

/*===========================================================================
FUNCTION   is_host_cmd

DESCRIPTION
  Filter ftm commands that do NOT need to be delegated to firmware

DEPENDENCIES
  NIL

RETURN VALUE
  Returns back whether command needs to be handled by host and not delegated to firmware

SIDE EFFECTS
  NONE

===========================================================================*/
static bool is_host_cmd(struct ftm_wlan_req_pkt_type *wlan_ftm_pkt)
{
   struct ftm_msg *msg;

   if (wlan_ftm_pkt->data == NULL) {
	   goto err_out;
   }
   msg = (struct ftm_msg *)wlan_ftm_pkt->data;

   switch (msg->msgId) {
   case MSG_GET_NV_TABLE:
   case MSG_SET_NV_TABLE:
   case MSG_GET_NV_FIELD:
   case MSG_SET_NV_FIELD:
   case MSG_STORE_NV_TABLE:
   case MSG_GET_NV_BIN:
   case MSG_SET_NV_BIN:
   case MSG_GET_DICTIONARY:
	   DPRINTF(FTM_DBG_INFO, "Host command 0x%04X\n", msg->msgId);
	   return true;
   default:
	   break;
   }

err_out:
	return false;
}

/*===========================================================================
FUNCTION   process_host_cmd

DESCRIPTION
  Process ftm commands locally e.g. relating to NV

DEPENDENCIES
  NIL

RETURN VALUE
  Returns back buffer that is meant to be passed to the diag callback

SIDE EFFECTS
  NONE

===========================================================================*/
static void *process_host_cmd(struct ftm_wlan_req_pkt_type *wlan_ftm_pkt, size_t pkt_len) {
	struct ftm_wlan_rsp_pkt_type *rsp = NULL;
	struct ftm_msg *msg;
	uint32_t magicNumber = 0;

	if (wlan_ftm_pkt->data == NULL) {
		DPRINTF(FTM_DBG_ERROR, "No data!\n");
		goto err_out;
	}

	msg = (struct ftm_msg*) wlan_ftm_pkt->data;
	rsp = (struct ftm_wlan_rsp_pkt_type*) diagpkt_subsys_alloc(sizeof(struct ftm_wlan_req_pkt_type) + msg->msgBodyLength);

	if (rsp == NULL) {
		DPRINTF(FTM_DBG_ERROR, "Failed to allocate Diag packet\n");
		goto err_out;
	}

	rsp->ftm_hdr = wlan_ftm_pkt->ftm_hdr;
    rsp->ftm_hdr.cmd_rsp_pkt_size = sizeof(struct ftm_wlan_req_pkt_type) + msg->msgBodyLength;
	rsp->result = FTM_ERR_CODE_FAIL;
	memcpy(rsp->data, msg, pkt_len - sizeof(struct ftm_wlan_req_pkt_type));
	msg = (struct ftm_msg *)rsp->data;

	if (ftm_config.nv == NULL) {
		if (read_nv_files()) {
			goto err_out;
		}
	}

	switch (msg->msgId) {
#if 1 // NV2 related commands
	case MSG_GET_NV_TABLE: {
		struct msg_get_nv_table *body = (struct msg_get_nv_table *)msg->msgResponse;
		sHalNvV2 *nvContents = (sHalNvV2 *) &g_nvContents;
		enum nv_version_type nvVersion = ftm_config.nvVersion;

		if (E_NV_V2 != nvVersion)
		{
			DPRINTF(FTM_DBG_ERROR, "%s : Not valid NV Version %d\n", "GET_NV_TABLE", nvVersion);
			return rsp;
		}
		DPRINTF(FTM_DBG_TRACE, "Returning NV table %d\n", body->nvTable);

		/* Test first chunk of NV table */
		if ((NV_MAX_TABLE == ftm_config.processingNVTable) ||
				(0 == ftm_config.processedNVTableSize))
		{
			/* Set Current Processing NV table type */
			ftm_config.processingNVTable = body->nvTable;

			switch (body->nvTable) {
				case NV_TABLE_RATE_POWER_SETTINGS:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.pwrOptimum);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.pwrOptimum;
				break;

				case NV_TABLE_REGULATORY_DOMAINS:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.regDomains);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.regDomains;
				break;

				case NV_TABLE_DEFAULT_COUNTRY:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.defaultCountryTable);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.defaultCountryTable;
				break;

				case NV_TABLE_TPC_POWER_TABLE:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.plutCharacterized);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.plutCharacterized[0];
				break;

				case NV_TABLE_TPC_PDADC_OFFSETS:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.plutPdadcOffset);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.plutPdadcOffset[0];
				break;

				case NV_TABLE_VIRTUAL_RATE:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.pwrOptimum_virtualRate);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.pwrOptimum_virtualRate[0];
				break;

				case NV_TABLE_RSSI_CHANNEL_OFFSETS:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.rssiChanOffsets);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.rssiChanOffsets[0];
				break;

				case NV_TABLE_HW_CAL_VALUES:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.hwCalValues);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.hwCalValues;
				break;

				case NV_TABLE_FW_CONFIG:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.fwConfig);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.fwConfig;
				break;

				case NV_TABLE_ANTENNA_PATH_LOSS:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.antennaPathLoss);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.antennaPathLoss[0];
				break;

				case NV_TABLE_PACKET_TYPE_POWER_LIMITS:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.pktTypePwrLimits);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.pktTypePwrLimits[0][0];
				break;

				default:
				DPRINTF(FTM_DBG_ERROR, "Not Valid NV Table %d", body->nvTable);
				return rsp;
				break;
			}

			/* Set Current Processing NV table type */
			ftm_config.processingNVTable = body->nvTable;
			/* Copy target NV table value into temp context buffer */
			memcpy(ftm_config.tempNVTableBuffer,
					ftm_config.targetNVTablePointer,
					ftm_config.targetNVTableSize);
		}
		/* Copy next chunk of NV table value into response buffer */
		memcpy(&body->tableData,
				ftm_config.tempNVTableBuffer + ftm_config.processedNVTableSize,
				body->chunkSize);
		/* Update processed pointer to prepare next chunk copy */
		ftm_config.processedNVTableSize += body->chunkSize;

		if (ftm_config.targetNVTableSize == ftm_config.processedNVTableSize)
		{
			/* Finished to process last chunk of data, initialize buffer */
			ftm_config.processingNVTable = NV_MAX_TABLE;
			ftm_config.targetNVTableSize = 0;
			ftm_config.processedNVTableSize = 0;
			memset(ftm_config.tempNVTableBuffer, 0, MAX_NV_TABLE_SIZE);
		}
		msg->respStatus = 0;
		rsp->result = FTM_ERR_CODE_PASS;

		break;
	}
	case MSG_SET_NV_TABLE: {
		struct msg_set_nv_table *body = (struct msg_set_nv_table *)msg->msgResponse;
		sHalNvV2 *nvContents = (sHalNvV2 *) &g_nvContents;
		DPRINTF(FTM_DBG_TRACE, "Setting NV table %d\n", body->nvTable);

		/* Test first chunk of NV table */
		if ((NV_MAX_TABLE == ftm_config.processingNVTable) ||
				(0 == ftm_config.processedNVTableSize))
		{
			/* Set Current Processing NV table type */
			ftm_config.processingNVTable = body->nvTable;

			switch (body->nvTable) {
				case NV_TABLE_RATE_POWER_SETTINGS:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.pwrOptimum);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.pwrOptimum;
				break;

				case NV_TABLE_REGULATORY_DOMAINS:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.regDomains);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.regDomains;
				break;

				case NV_TABLE_DEFAULT_COUNTRY:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.defaultCountryTable);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.defaultCountryTable;
				break;

				case NV_TABLE_TPC_POWER_TABLE:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.plutCharacterized);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.plutCharacterized[0];
				break;

				case NV_TABLE_TPC_PDADC_OFFSETS:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.plutPdadcOffset);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.plutPdadcOffset[0];
				break;

				case NV_TABLE_VIRTUAL_RATE:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.pwrOptimum_virtualRate);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.pwrOptimum_virtualRate[0];
				break;

				case NV_TABLE_RSSI_CHANNEL_OFFSETS:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.rssiChanOffsets);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.rssiChanOffsets[0];
				break;

				case NV_TABLE_HW_CAL_VALUES:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.hwCalValues);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.hwCalValues;
				break;

				case NV_TABLE_FW_CONFIG:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.fwConfig);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.fwConfig;
				break;

				case NV_TABLE_ANTENNA_PATH_LOSS:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.antennaPathLoss);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.antennaPathLoss[0];
				break;

				case NV_TABLE_PACKET_TYPE_POWER_LIMITS:
				ftm_config.targetNVTableSize = sizeof(nvContents->tables.pktTypePwrLimits);
				ftm_config.targetNVTablePointer = (uint8_t *)&nvContents->tables.pktTypePwrLimits[0][0];
				break;

				default:
				DPRINTF(FTM_DBG_ERROR, "Not Valid NV Table %d", body->nvTable);
				return rsp;
				break;
			}

			/* Set Current Processing NV table type */
			ftm_config.processingNVTable = body->nvTable;
		}

		/* Copy next chunk of NV table value from response buffer */
		memcpy(ftm_config.tempNVTableBuffer + ftm_config.processedNVTableSize,
				&body->tableData,
				body->chunkSize);
		/* Update processed pointer to prepare next chunk copy */
		ftm_config.processedNVTableSize += body->chunkSize;

		if (ftm_config.targetNVTableSize == ftm_config.processedNVTableSize)
		{
			memcpy(ftm_config.targetNVTablePointer,
					ftm_config.tempNVTableBuffer,
					ftm_config.targetNVTableSize);
			/* Finished to process last chunk of data, initialize buffer */
			ftm_config.processingNVTable = NV_MAX_TABLE;
			ftm_config.targetNVTableSize = 0;
			ftm_config.processedNVTableSize = 0;
			memset(ftm_config.tempNVTableBuffer, 0, MAX_NV_TABLE_SIZE);
		}
		msg->respStatus = 0;
		rsp->result = FTM_ERR_CODE_PASS;

		break;
	}
#endif
#if 1 // NV3 related commands
	case MSG_GET_NV_FIELD: {
		struct msg_get_nv_field *body =
				(struct msg_get_nv_field *) msg->msgResponse;
		sHalNv *nvContents = &g_nvContents;
		DPRINTF(FTM_DBG_TRACE, "Getting NV field %d\n",
				body->nvField);

		switch (body->nvField) {
		case NV_COMMON_PRODUCT_ID:
			memcpy((void *) &body->fieldData, &nvContents->fields.productId,
					sizeof(nvContents->fields.productId));
			break;

		case NV_COMMON_PRODUCT_BANDS:
			memcpy((void *) &body->fieldData, &nvContents->fields.productBands,
					sizeof(nvContents->fields.productBands));
			break;

		case NV_COMMON_NUM_OF_TX_CHAINS:
			memcpy((void *) &body->fieldData, &nvContents->fields.numOfTxChains,
					sizeof(nvContents->fields.numOfTxChains));
			break;

		case NV_COMMON_NUM_OF_RX_CHAINS:
			memcpy((void *) &body->fieldData, &nvContents->fields.numOfRxChains,
					sizeof(nvContents->fields.numOfRxChains));
			break;

		case NV_COMMON_MAC_ADDR:
			memcpy((void *) &body->fieldData, &nvContents->fields.macAddr[0],
			NV_FIELD_MAC_ADDR_SIZE);
			break;

		case NV_COMMON_MFG_SERIAL_NUMBER:
			memcpy((void *) &body->fieldData, &nvContents->fields.mfgSN[0],
			NV_FIELD_MFG_SN_SIZE);
			break;

		case NV_COMMON_WLAN_NV_REV_ID:
			memcpy((void *) &body->fieldData, &nvContents->fields.wlanNvRevId,
					sizeof(nvContents->fields.wlanNvRevId));
			break;

		case NV_COMMON_COUPLER_TYPE:
			memcpy((void *) &body->fieldData, &nvContents->fields.couplerType,
					sizeof(nvContents->fields.couplerType));
			break;

		case NV_COMMON_NV_VERSION: {
			uint8_t nvVersion = nvContents->fields.nvVersion;
			DPRINTF(FTM_DBG_DUMP, "nvVersion = %d\n", nvVersion);
			bool nvEmbedded = (ftm_config.nvVersion == E_NV_V3);

			if (nvEmbedded) {
				// High bit is set to indicate embedded NV..
				nvVersion = nvVersion | NV_EMBEDDED_VERSION;
			}

			DPRINTF(FTM_DBG_TRACE, "%sVersion=%d\n",
					nvEmbedded ? "embedded " : "", nvVersion);

			memcpy((void *) &body->fieldData, &nvVersion,
					sizeof(nvContents->fields.nvVersion));
		}
			break;

		default:
			DPRINTF(FTM_DBG_ERROR, "Not Valid NV field %d", body->nvField);
			return rsp;
			break;
		}
		msg->respStatus = 0;
		rsp->result = FTM_ERR_CODE_PASS;

		break;
	}
	case MSG_SET_NV_FIELD: {
		struct msg_set_nv_field *body =
				(struct msg_set_nv_field *) msg->msgResponse;
		sHalNv *nvContents = &g_nvContents;
		DPRINTF(FTM_DBG_TRACE, "Setting NV field %d\n",
				body->nvField);

		switch (body->nvField) {
		case NV_COMMON_PRODUCT_ID:
			memcpy(&nvContents->fields.productId, &body->fieldData,
					sizeof(nvContents->fields.productId));
			break;

		case NV_COMMON_PRODUCT_BANDS:
			memcpy(&nvContents->fields.productBands, &body->fieldData,
					sizeof(nvContents->fields.productBands));
			break;

		case NV_COMMON_NUM_OF_TX_CHAINS:
			memcpy(&nvContents->fields.numOfTxChains, &body->fieldData,
					sizeof(nvContents->fields.numOfTxChains));
			break;

		case NV_COMMON_NUM_OF_RX_CHAINS:
			memcpy(&nvContents->fields.numOfRxChains, &body->fieldData,
					sizeof(nvContents->fields.numOfRxChains));
			break;

		case NV_COMMON_MAC_ADDR: {
			uint8_t macLoop;
			uint8_t *pNVMac;
			uint8_t lastByteMAC;
//			 /* If Last byte is larger than 252 (0xFC), return Error,
//			  * Since 3MACs should be derived from first MAC */
//			 if(QWLAN_MAX_MAC_LAST_BYTE_VALUE <
//				body->fieldData.macAddr.macAddr1[NV_FIELD_MAC_ADDR_SIZE - 1])
//			 {
//				VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
//						   "Last Byte of the seed MAC is too large 0x%x",
//							body->fieldData.macAddr.macAddr1[NV_FIELD_MAC_ADDR_SIZE - 1]);
//				return -EILSEQ;
//			 }

			pNVMac = (uint8_t *) nvContents->fields.macAddr;
			lastByteMAC =
					body->fieldData.macAddr.macAddr1[NV_FIELD_MAC_ADDR_SIZE - 1];
			for (macLoop = 0; macLoop < 4; macLoop++) {
				memcpy(pNVMac + (macLoop * NV_FIELD_MAC_ADDR_SIZE),
						&body->fieldData.macAddr.macAddr1[0],
						NV_FIELD_MAC_ADDR_SIZE - 1);
				(pNVMac + (macLoop * NV_FIELD_MAC_ADDR_SIZE))[NV_FIELD_MAC_ADDR_SIZE
						- 1] = lastByteMAC + macLoop;
			}
			break;
		}

		case NV_COMMON_MFG_SERIAL_NUMBER:
			memcpy(&nvContents->fields.mfgSN[0], &body->fieldData,
			NV_FIELD_MFG_SN_SIZE);
			break;

		case NV_COMMON_WLAN_NV_REV_ID:
			memcpy(&nvContents->fields.wlanNvRevId, &body->fieldData,
					sizeof(nvContents->fields.wlanNvRevId));
			break;

		case NV_COMMON_COUPLER_TYPE:
			memcpy(&nvContents->fields.couplerType, &body->fieldData,
					sizeof(nvContents->fields.couplerType));
			break;

		case NV_COMMON_NV_VERSION:
			DPRINTF(FTM_DBG_ERROR, "Cannot modify NV version field %d", body->nvField);
			return rsp;
			break;

		default:
			DPRINTF(FTM_DBG_ERROR, "Not Valid NV field %d", body->nvField);
			return rsp;
			break;
		}
		msg->respStatus = 0;
		rsp->result = FTM_ERR_CODE_PASS;

		break;
	}
	case MSG_STORE_NV_TABLE: {
		struct msg_store_nv_table* body =
				(struct msg_store_nv_table *) msg->msgResponse;
		DPRINTF(FTM_DBG_TRACE, "NOT Storing NV table %d\n",
				body->nvTable);
		if (write_nv_files()) {
			DPRINTF(FTM_DBG_ERROR, "Failed to persist NV file\n");
			return rsp;
		}
		msg->respStatus = 0;
		rsp->result = FTM_ERR_CODE_PASS;

		break;
	}
	case MSG_GET_NV_BIN:
	case MSG_GET_DICTIONARY: {
		struct msg_get_nv_table *body =
				(struct msg_get_nv_table *) msg->msgResponse;
		size_t nvSize = 0;
		uint16_t offset = 0;
		enum nv_version_type nvVersion = ftm_config.nvVersion;

		if (E_NV_V3 != nvVersion) {
			DPRINTF(FTM_DBG_ERROR, "%s : Not valid NV Version %d", "GET_NV_BIN", nvVersion);
			return rsp;
		}
		DPRINTF(FTM_DBG_TRACE, "Returning NV table %d\n",
				body->nvTable);
		if ((NV_MAX_TABLE == ftm_config.processingNVTable)
				|| (0 == ftm_config.processedNVTableSize)) {
			switch (body->nvTable) {
			case NV_BINARY_IMAGE:
				if (msg->msgId == MSG_GET_NV_BIN) {
					ftm_config.targetNVTablePointer =
							(uint8_t *)&ftm_config.nv->magic_number;
					nvSize = ftm_config.nvSize;
				} else {
					ftm_config.targetNVTablePointer =
							ftm_config.dict;
					nvSize = ftm_config.dictSize;
				}
				break;
			default:
				DPRINTF(FTM_DBG_ERROR, "Not Valid NV Table %d", body->nvTable);
				return rsp;
				break;
			}

			/* Set Current Processing NV table type */
			ftm_config.processingNVTable = body->nvTable;
			if (msg->msgId == MSG_GET_NV_BIN) {
				ftm_config.targetNVTableSize = sizeof(uint32_t) + nvSize; // magic_number + data
				/* Validity Period */
				ftm_config.tempNVTableBuffer[0] = 0xFF;
				ftm_config.tempNVTableBuffer[1] = 0xFF;
				ftm_config.tempNVTableBuffer[2] = 0xFF;
				ftm_config.tempNVTableBuffer[3] = 0xFF;
				offset = sizeof(uint32_t); // skip is_valid
			} else {
				ftm_config.targetNVTableSize = nvSize;
				offset = 0;
			}

			DPRINTF(FTM_DBG_TRACE,
					"nvSize=%d, targetNVTableSize=%d, Offset=%d\n", nvSize,
					ftm_config.targetNVTableSize, offset);
			/* Copy target NV table value into temp context buffer */
			memcpy(&ftm_config.tempNVTableBuffer[offset],
					ftm_config.targetNVTablePointer,
					ftm_config.targetNVTableSize);
		}

		body->tableSize = ftm_config.targetNVTableSize;

		/* Update processed pointer to prepare next chunk copy */
		if ((body->chunkSize + ftm_config.processedNVTableSize)
				> ftm_config.targetNVTableSize) {
			body->chunkSize = (ftm_config.targetNVTableSize
					- ftm_config.processedNVTableSize);
		}

		DPRINTF(FTM_DBG_TRACE,
				"Copying to table data from offset=%d chunk=%d bytes\n",
				ftm_config.processedNVTableSize, body->chunkSize);
		/* Copy next chunk of NV table value into response buffer */
		memcpy(&body->tableData,
				ftm_config.tempNVTableBuffer + ftm_config.processedNVTableSize,
				body->chunkSize);

		ftm_config.processedNVTableSize += body->chunkSize;

		if (ftm_config.targetNVTableSize == ftm_config.processedNVTableSize) {
			/* Finished to process last chunk of data, initialize buffer */
			ftm_config.processingNVTable = NV_MAX_TABLE;
			ftm_config.targetNVTableSize = 0;
			ftm_config.processedNVTableSize = 0;
			memset(ftm_config.tempNVTableBuffer, 0, MAX_NV_TABLE_SIZE);
		}
		msg->respStatus = 0;
		rsp->result = FTM_ERR_CODE_PASS;

		break;
	}
	case MSG_SET_NV_BIN: {
		struct msg_set_nv_table* body =
				(struct msg_set_nv_table *) msg->msgResponse;
		enum nv_version_type nvVersion = ftm_config.nvVersion;

		if (E_NV_V3 != nvVersion) {
			DPRINTF(FTM_DBG_ERROR, "%s : Not valid NV Version %d", "GET_NV_BIN", nvVersion);
			return rsp;
		}
		DPRINTF(FTM_DBG_TRACE, "Setting NV table %d\n",
				body->nvTable);
		ftm_config.targetNVTablePointer =
				(uint8_t *)ftm_config.nv;

		/* Test first chunk of NV table */
		if ((NV_MAX_TABLE == ftm_config.processingNVTable) ||
		   (0 == ftm_config.processedNVTableSize))
		{
		  switch (body->nvTable)
		  {
			 case NV_BINARY_IMAGE:
				ftm_config.targetNVTableSize = body->tableSize;
				break;
			 default:
				DPRINTF(FTM_DBG_ERROR, "Not Valid NV Table %d", body->nvTable);
				return rsp;
				break;
		  }

		  /* Set Current Processing NV table type */
		  ftm_config.processingNVTable = body->nvTable;
		  ftm_config.processedNVTableSize = 0;

		  if (ftm_config.targetNVTableSize != body->tableSize)
		  {
			 	DPRINTF(FTM_DBG_ERROR, "Invalid Table Size %d", body->tableSize);
				ftm_config.processingNVTable = NV_MAX_TABLE;
				ftm_config.targetNVTableSize = 0;
				ftm_config.processedNVTableSize = 0;
				memset(ftm_config.tempNVTableBuffer, 0, MAX_NV_TABLE_SIZE);
				return rsp;
		  }
		}

		if (ftm_config.processingNVTable != body->nvTable)
		{
		  	DPRINTF(FTM_DBG_ERROR, "Invalid NV Table, now Processing %d, not %d",
					  ftm_config.processingNVTable, body->nvTable);
			ftm_config.processingNVTable = NV_MAX_TABLE;
			ftm_config.targetNVTableSize = 0;
			ftm_config.processedNVTableSize = 0;
			memset(ftm_config.tempNVTableBuffer, 0, MAX_NV_TABLE_SIZE);
			return rsp;
		}

		memcpy(ftm_config.tempNVTableBuffer + ftm_config.processedNVTableSize,
		   &body->tableData,
		   body->chunkSize);

		ftm_config.processedNVTableSize += body->chunkSize;

		if (ftm_config.targetNVTableSize == ftm_config.processedNVTableSize)
		{
		  	DPRINTF(FTM_DBG_TRACE,
			 "Processing Done!! write encoded Buffer %d",
			  ftm_config.targetNVTableSize);

		  	memcpy(ftm_config.targetNVTablePointer, ftm_config.tempNVTableBuffer, ftm_config.targetNVTableSize);
			ftm_config.processingNVTable = NV_MAX_TABLE;
			ftm_config.targetNVTableSize = 0;
			ftm_config.processedNVTableSize = 0;
			memset(ftm_config.tempNVTableBuffer, 0, MAX_NV_TABLE_SIZE);
		}
		msg->respStatus = 0;
		rsp->result = FTM_ERR_CODE_PASS;

		break;
	}
#endif
	} // end of switch

err_out:
	return rsp;
}

/*===========================================================================
FUNCTION   ftm_wlan_common_op

DESCRIPTION
  Process ftm commands like load driver, Tx, Rx and few test commands

DEPENDENCIES
  NIL

RETURN VALUE
  Returns back buffer that is meant to be passed to the diag callback

SIDE EFFECTS
  NONE

===========================================================================*/
static void *ftm_wlan_common_op(struct ftm_wlan_req_pkt_type *wlan_ftm_pkt, size_t pkt_len)
{
   char ifname[IFNAMSIZ];
   int ifindex = 0;
   bool resp = false;
   struct ftm_wlan_rsp_pkt_type *rsp;
   size_t req_len = pkt_len - sizeof(struct diagpkt_subsys_header_v2_type); // wlan_request_buffer length

   DPRINTF(FTM_DBG_TRACE, "Started %s\n", __FUNCTION__);
   snprintf(ifname, sizeof(ifname), "%s%d", g_ifname, ifindex);

   if (req_len <= 0) {
       DPRINTF(FTM_DBG_ERROR, "Invalid req_len: %d\n", req_len);
       return NULL;
   }

   DPRINTF(FTM_DBG_TRACE, "Command ID rec'd: 0x%02X + data length %d\n", wlan_ftm_pkt->ftm_cmd_type, req_len - sizeof(wlan_ftm_pkt->ftm_cmd_type));

   if (is_host_cmd(wlan_ftm_pkt)) { // Commands that are handled without delegation to firmware
	   rsp = process_host_cmd(wlan_ftm_pkt, pkt_len);
	   return (void *) rsp;
   } else { // Commands that are delegated to firmware via driver
	   g_rsp = NULL;

	   rsp = (struct ftm_wlan_rsp_pkt_type*)diagpkt_subsys_alloc(sizeof(struct ftm_wlan_req_pkt_type));

	   if (rsp == NULL) {
	           DPRINTF(FTM_DBG_ERROR, "Failed to allocate Diag packet\n");
	           goto err_out;
	   }

	   rsp->ftm_hdr = wlan_ftm_pkt->ftm_hdr;
	   rsp->result = FTM_ERR_CODE_PASS;

	   // Force reinitialisation! TODO: Fix this hack
	   ifs_init[ifindex] = false;
	   if (!ifs_init[ifindex])
	   {
		   DPRINTF(FTM_DBG_TRACE, "Initializing Interface: %s\n", ifname);

		   if (tcmd_tx_init(ifname, ftm_wlan_tcmd_rx))
		   {
			   DPRINTF(FTM_DBG_ERROR, "Couldn't init tcmd transport!\n");
			   rsp->result = FTM_ERR_CODE_FAIL;
			   goto err_out;
		   }

		   DPRINTF(FTM_DBG_TRACE, "Initialized Interface: %s\n", ifname);
		   ifs_init[ifindex] = true;
	   } else {
		   DPRINTF(FTM_DBG_TRACE, "Interface: %s already initialized\n", ifname);
	   }

	   resp = (wlan_ftm_pkt->ftm_cmd_type == WCN36XX_TM_CMD_PTT);
	   g_req = wlan_ftm_pkt;
	   if (tcmd_tx(wlan_ftm_pkt->ftm_cmd_type, (void *)wlan_ftm_pkt->data, req_len - sizeof(wlan_ftm_pkt->ftm_cmd_type), resp))
	   {
			DPRINTF(FTM_DBG_ERROR, "TCMD timed out!\n");
			rsp->result = FTM_ERR_CODE_FAIL;
			goto err_out;
	   }

	   if (resp)
	   {
		   if (g_rsp)
		   {
			   free(rsp);
			   DPRINTF(FTM_DBG_ERROR, "Global response received from callback\n");
			   return (void *) g_rsp;
		   }
		   else
		   {
			   DPRINTF(FTM_DBG_ERROR, "No response got probably timing out.... \n");
			   rsp->result = FTM_ERR_CODE_FAIL;
			   goto err_out;
		   }
	   }
   }

err_out:
	DPRINTF(FTM_DBG_TRACE, "Default response!\n");
    return (void *) rsp;
}

/*=====================================================================
 Qdiag Extension API
=====================================================================*/

int qdiag_handle_ftm_wlan(void *dc,void * data)
{
	struct mbuf *mbuf = (struct mbuf*)data;
	uint8_t *msg;
	int ret = 0;
	int i = 0;
	size_t req_len = 0, rsp_len = 0;
	struct ftm_wlan_req_pkt_type *req_pkt;
	struct ftm_wlan_rsp_pkt_type *rsp_pkt = NULL;

	msg = (uint8_t *)mbuf->data;
	req_len = mbuf->offset;
	req_pkt =(struct ftm_wlan_req_pkt_type *)msg;
    if (!req_pkt || !req_len) {
        DPRINTF(FTM_DBG_ERROR, "Invalid ftm wlan Request Packet\n");
        return 1;
    }

    DPRINTF(FTM_DBG_DUMP, "Request Packet length=%lu:\n", req_len);
    DPRINTF(FTM_DBG_TRACE, "req_pkt->diag_cmd = 0x%02X%02X%04X\n",req_pkt->diag_hdr.cmd_code, req_pkt->diag_hdr.subsys_id, req_pkt->diag_hdr.subsys_cmd_code);
    DPRINTF(FTM_DBG_TRACE, "req_pkt->cmd_id = 0x%04X\n",req_pkt->ftm_hdr.cmd_id);
    DPRINTF(FTM_DBG_TRACE, "req_pkt->cmd_data_len = 0x%04X\n",req_pkt->ftm_hdr.cmd_data_len);
    DPRINTF(FTM_DBG_TRACE, "req_pkt->cmd_rsp_pkt_size = 0x%04X\n",req_pkt->ftm_hdr.cmd_rsp_pkt_size);
    DPRINTF(FTM_DBG_TRACE, "req_pkt->module_type = 0x%04X\n",req_pkt->module_type);
    DPRINTF(FTM_DBG_TRACE, "req_pkt->ftm_cmd_type = 0x%04X\n",req_pkt->ftm_cmd_type);
    DPRINTF(FTM_DBG_TRACE, "req_pkt->cmd_data %lu bytes:\n", req_pkt->ftm_hdr.cmd_data_len - sizeof(req_pkt->module_type));
//    print_uchar_array(req_pkt->data, req_pkt->ftm_hdr.cmd_data_len - sizeof(req_pkt->module_type));
    print_uchar_array((uint8_t *)req_pkt, req_len);

    if (req_pkt->module_type != QUALCOMM_MODULE_NUMBER) {
 	   DPRINTF(FTM_DBG_ERROR, "Invalid module_type: %d should be %d\n", req_pkt->module_type, QUALCOMM_MODULE_NUMBER);
 	   return 1;
    }

    switch (req_pkt->ftm_hdr.cmd_id) {
    case FTM_WLAN_COMMON_OP:
    	rsp_pkt = (struct ftm_wlan_rsp_pkt_type *)ftm_wlan_common_op(req_pkt, req_len);
        if (rsp_pkt != NULL) {
        	rsp_len = rsp_pkt->ftm_hdr.cmd_rsp_pkt_size;
        	if (rsp_len == 0) {
        		rsp_len = sizeof(*req_pkt);
        	} else {
        		rsp_pkt->ftm_hdr.cmd_rsp_pkt_size = 0;
        	}
        }
        break;
    default:
        DPRINTF(FTM_DBG_ERROR, "Unknown Command\n");
        return 1;
    }
	if (rsp_pkt == NULL) {
		DPRINTF(FTM_DBG_ERROR, "No response!!!\n");
		return 1;
	}

    DPRINTF(FTM_DBG_DUMP, "Response Packet length=%lu:\n", rsp_len);
    DPRINTF(FTM_DBG_TRACE, "rsp_pkt->diag_cmd = 0x%02X%02X%04X\n",rsp_pkt->diag_hdr.cmd_code, rsp_pkt->diag_hdr.subsys_id, rsp_pkt->diag_hdr.subsys_cmd_code);
    DPRINTF(FTM_DBG_TRACE, "rsp_pkt->cmd_id = 0x%04X\n",rsp_pkt->ftm_hdr.cmd_id);
    DPRINTF(FTM_DBG_TRACE, "rsp_pkt->cmd_data_len = 0x%04X\n",rsp_pkt->ftm_hdr.cmd_data_len);
    DPRINTF(FTM_DBG_TRACE, "rsp_pkt->cmd_rsp_pkt_size = 0x%04X\n",rsp_pkt->ftm_hdr.cmd_rsp_pkt_size);
    DPRINTF(FTM_DBG_TRACE, "rsp_pkt->result = 0x%04X%s\n",rsp_pkt->result,(rsp_pkt->result?"      ERROR!!!":""));
    DPRINTF(FTM_DBG_TRACE, "rsp_pkt->cmd_data %lu bytes:\n", rsp_len - sizeof(*rsp_pkt));
//    print_uchar_array(rsp_pkt->data, rsp_len);
    print_uchar_array((uint8_t *)rsp_pkt, rsp_len);

    ret = ftm_diag_encode_send((uint8_t *)rsp_pkt, rsp_len);

    return 0;
}

void qdiag_get_table(struct qdiag_table **tbl_ptr)
{
	struct qdiag_table *ptr=NULL;
	g_entry.first_cmd = g_entry.last_cmd = // shift bytes so cmd_code is MSB
		(((uint32_t)g_diag_hdr.cmd_code << 24) | ((uint32_t)g_diag_hdr.subsys_id << 16) | ((uint32_t)g_diag_hdr.subsys_cmd_code << 0));
	g_entry.cb = &qdiag_handle_ftm_wlan;
	ptr=(struct qdiag_table *)malloc(sizeof(struct qdiag_table));
	ptr->hdr.num_of_entries=1;
	ptr->table=(struct qdiag_table_entry *)malloc(sizeof(struct qdiag_table_entry));
	memcpy(ptr->table,&g_entry,sizeof(struct qdiag_table_entry));
	*tbl_ptr=ptr;
	return ;
}

int qdiag_set_pipe(int fd)
{
	qdiag_fd = fd;
	DPRINTF(FTM_DBG_INFO, "qdiag_fd = %d\n", qdiag_fd);
	
	return 0;
}

int qdiag_set_debug_level(int level)
{
	if (!level) {
		g_dbg_level = level;
	}
	DPRINTF(FTM_DBG_INFO, "g_dbg_level = %d\n", g_dbg_level);

	return 0;
}
