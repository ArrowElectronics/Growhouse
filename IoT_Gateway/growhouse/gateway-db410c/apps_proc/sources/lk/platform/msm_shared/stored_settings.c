/* Copyright (c) 2017, The Linux Foundation. All rights reserved.
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

#include <stored_settings.h>
#include <partition_parser.h>
#include <diskio.h>
#include <ff.h>
#include <err.h>
#include <mmc_wrapper.h>
#include <debug.h>

#define SECTOR_SIZE 512

#define BT_PROVISION_FILE "/QCOM/BT"
#define WLAN_PROVISION_FILE "/QCOM/WLAN"

static struct wlan_provision g_wlan_provision;
static struct bt_provision g_bt_provision;

static void apply_wlan_provision();
static void apply_bt_provision();
static FATFS fs;


// FatFs IO layer implementation
DSTATUS disk_status(BYTE pdrv)
{
	return RES_OK;
}

DSTATUS disk_initialize(BYTE pdrv)
{
	return RES_OK;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
	unsigned long long partition_offset = partition_get_dpp_offset();

	return mmc_read_boot2(partition_offset + (sector * SECTOR_SIZE), buff, count * SECTOR_SIZE);
}


int read_stored_settings()
{
	int ret;
	FIL fil;
	FRESULT res;

	res = f_mount(&fs, "", 1);
    if (res != FR_OK)
	{
        dprintf(CRITICAL, "ERROR: Can't mount DPP on BOOT2 partition\n");
		return 1;
    }

	res = f_open(&fil, WLAN_PROVISION_FILE, FA_READ);
	if (res != FR_OK)
	{
		dprintf(CRITICAL, "ERROR: Can't open WLAN provision file\n");
	}
	else
	{
		res = f_read(&fil, &g_wlan_provision, sizeof(struct wlan_provision), &ret);
		if ((res != FR_OK) || (ret != sizeof(struct wlan_provision)))
		{
			dprintf(CRITICAL, "Unexpected end of file: %s %d\n", WLAN_PROVISION_FILE, ret);
		}
		f_close(&fil);
	}

	res = f_open(&fil, BT_PROVISION_FILE, FA_READ);
	if (res != FR_OK)
	{
		dprintf(CRITICAL, "ERROR: Can't open BT provision file\n");
	}
	else
	{
		res = f_read(&fil, &g_bt_provision, sizeof(struct bt_provision), &ret);
		if ((res != FR_OK) || (ret != sizeof(struct bt_provision)))
		{
			dprintf(CRITICAL, "Unexpected end of file: %s %d\n", BT_PROVISION_FILE, ret);
		}
		f_close(&fil);
	}

	return NO_ERROR;
}


static bool mac_invalid(char * mac)
{
	/* If all the mac bytes are zeroed, we didn't manage to read any mac from storage */
	for (int i=0; i<6; i++)
	{
		if (mac[i] != 0)
			return false;
	}

	return true;
}

/* Function to read WLAN MAC from secure storage
 * Returns 0: if read failed, 1: on success
 */
int stored_settings_get_wlan_mac(char * mac)
{
	if (!mac_invalid(g_wlan_provision.sta_mac))
	{
		memcpy(mac, g_wlan_provision.sta_mac, 6);
		return 1;
	}

	return 0;
}

/* Function to read BT MAC from secure storage
 * Returns 0: if read failed, 1: on success
 */
int stored_settings_get_bt_mac(char * mac)
{
	if (!mac_invalid(g_bt_provision.bd_address))
	{
		memcpy(mac, g_bt_provision.bd_address, 6);
		return 1;
	}

	return 0;
}


