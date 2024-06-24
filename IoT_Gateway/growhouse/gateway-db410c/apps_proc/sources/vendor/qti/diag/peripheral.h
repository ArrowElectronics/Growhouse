#ifndef __PERIPHERAL_H__
#define __PERIPHERAL_H__

#include "list.h"

typedef enum peripheral_ch_type{
	peripheral_ch_type_data = 0,
	peripheral_ch_type_ctrl,
	peripheral_ch_type_cmd,
	MAX_NUM_OF_CH
}peripheral_ch_type;

struct peripheral {
	struct list_head  node;

	char *name;
	char *ch_name[MAX_NUM_OF_CH];

	unsigned long features;

	struct list_head cntlq;
	struct list_head dataq;
	struct list_head cmdq;

	int fd[MAX_NUM_OF_CH];
};

extern struct list_head *peripherals;

int peripheral_init(struct list_head *,struct list_head * );
void peripheral_close(struct peripheral *peripheral);
int peripheral_exit();
#endif
