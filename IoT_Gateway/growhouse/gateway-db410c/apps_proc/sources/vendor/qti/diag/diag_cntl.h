#ifndef __DIAG_CNTL_H__
#define __DIAG_CNTL_H__

#include "qdiag.h"
#include "masks.h"

int diag_cntl_recv(int fd, void *data);
void diag_cntl_send_feature_mask(struct peripheral *peripheral);
void diag_cntl_close(struct peripheral *peripheral);

/* LOG CODES */
static const uint32_t log_code_last_tbl[] = {
	0x0,	/* EQUIP ID 0 */
	0x1A02,	/* EQUIP ID 1 */
	0x0,	/* EQUIP ID 2 */
	0x0,	/* EQUIP ID 3 */
	0x4910,	/* EQUIP ID 4 */
	0x5420,	/* EQUIP ID 5 */
	0x0,	/* EQUIP ID 6 */
	0x74FF,	/* EQUIP ID 7 */
	0x0,	/* EQUIP ID 8 */
	0x0,	/* EQUIP ID 9 */
	0xA38A,	/* EQUIP ID 10 */
	0xB201,	/* EQUIP ID 11 */
	0x0,	/* EQUIP ID 12 */
	0xD1FF,	/* EQUIP ID 13 */
	0x0,	/* EQUIP ID 14 */
	0x0,	/* EQUIP ID 15 */
};

#define LOG_GET_ITEM_NUM(xx_code)	(xx_code & 0x0FFF)
#define LOG_GET_EQUIP_ID(xx_code)	((xx_code & 0xF000) >> 12)
#define LOG_ITEMS_TO_SIZE(num_items)	((num_items+7)/8)
#define LOG_SIZE_TO_ITEMS(size)		((8*size) - 7)
#define EVENT_COUNT_TO_BYTES(count)	((count+7)/8)

#define MAX_EQUIP_ID	16
#define EVENT_MASK_SIZE 513
#define MAX_ITEMS_PER_EQUIP_ID	512
#define MAX_ITEMS_ALLOWED	0xFFF

#define LOG_MASK_CTRL_HEADER_LEN	11
#define MSG_MASK_CTRL_HEADER_LEN	11
#define EVENT_MASK_CTRL_HEADER_LEN	7

#define LOG_STATUS_SUCCESS	0
#define LOG_STATUS_INVALID	1
#define LOG_STATUS_FAIL		2

#define MSG_STATUS_FAIL		0
#define MSG_STATUS_SUCCESS	1

#define EVENT_STATUS_SUCCESS	0
#define EVENT_STATUS_FAIL	1

#define DIAG_CTRL_MASK_INVALID		0
#define DIAG_CTRL_MASK_ALL_DISABLED	1
#define DIAG_CTRL_MASK_ALL_ENABLED	2
#define DIAG_CTRL_MASK_VALID		3


void diag_cntl_send_log_mask(struct peripheral *peripheral, uint32_t equip_id);
void diag_cntl_send_msg_mask(struct peripheral *peripheral, struct diag_ssid_range_t *range);
void diag_cntl_send_event_mask(struct peripheral *peripheral);

#endif // __DIAG_CNTL_H__
