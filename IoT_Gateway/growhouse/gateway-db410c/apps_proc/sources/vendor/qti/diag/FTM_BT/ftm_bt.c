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
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <sys/select.h>
#include "hdlc.h"
#include "mbuf.h"

/* Reader thread handle */
pthread_t hci_cmd_thread_hdl;

/* rpmsg BT command channel to open */
#define  BT_RPMSG_CMD_CHANNEL "/dev/rpmsg/pronto/APPS_RIVA_BT_CMD"

#define DIAG_LOG_F      16

int bt_fd=0;
int hdev=0;
int qdiag_handle_ftm_bt(void *dc,void * data);
int qdiag_fd=0;
struct qdiag_table_entry entry={0x4B0B0004,0x4B0B00FF,&qdiag_handle_ftm_bt};
struct qdiag_table test_table={{1},&entry};

int debug_op=0;

typedef struct
{
  uint8_t cmd_code;
  uint8_t more;   /* Indicates how many log entries, not including the one
                   returned with this packet, are queued up in the Mobile
                   Station.  If DIAG_DIAGVER >= 8, this should be set to 0 */
  uint16_t len;   /* Indicates the length, in bytes, of the following log entry */
  uint8_t *log; /* Contains the log entry data. */
}
diag_log_rsp_type;

#define BT_HCI_CMD_PKT 1
#define HCI_CMD_HDR_SIZE 3
#define HC_VS_MAX_CMD_EVENT 256

#define BT_QSOC_EDL_CMD_OPCODE             (0xFC00)
#define BT_QSOC_NVM_ACCESS_OPCODE          (0xFC0B)
#define BT_QSOC_READ_BD_ADDR_OPCODE        (0x1009)
#define CMD_WRITE_BD_ADDRESS			   (0xFC31)
#define OGF_INFO_PARAM			0x04
#define OCF_READ_BD_ADDR		0x0009
#define FTM_BT_CMD_NV_READ		0x0B
#define NV_BD_ADDR_I		0x01

#define BT_QSOC_EDL_CMD_CODE             (0x00)
#define BT_QSOC_NVM_ACCESS_CODE          (0x0B)
#define BT_QSOC_VS_EDL_APPVER_RESP   	(0x02)
 
 
#define FTM_BT_DIAG_CODE 0x117C
#define FTM_BT_EVENT_CODE 0x1366
#define FTM_BT_CMD_CODE 0x1365
 
int ftm_diag_encode_send(uint8_t *pkt, uint8_t length)
{
	int i;
	uint8_t *outbuf = NULL;
	size_t outlen = length;
	
	printf("%s,%d: ",__FUNCTION__,__LINE__);
	for(i=0;i<outlen;printf("0x%x ",pkt[i++]));
	printf("errno = %d %s,%d: \n",errno,__FUNCTION__,__LINE__);
	
	outbuf = hdlc_encode(pkt, length, &outlen);
	return write(qdiag_fd,outbuf, outlen);
}

int ftm_bt_send_log_packet(uint16_t type,uint8_t *pkt, uint8_t length)
{
	/*diag_log_rsp_type*/ uint8_t *log_rsp_pkt;//={DIAG_LOG_F,0x00,0x00,0};
	int i;
	int len=length+4+12;
	int ret=0;
	uint64_t timestamp=(int)time(NULL);
		
	log_rsp_pkt=malloc(len);
	//#define FTM_LOG_CODE 0x117c
	for(i=0;i<length;printf("0x%x ",pkt[i++]));
	printf("timestamp = %lX %s,%d: \n",timestamp,__FUNCTION__,__LINE__);
	//memcpy(&log_rsp_pkt[8],&timestamp,sizeof(timestamp));
	log_rsp_pkt[0]=DIAG_LOG_F;
	log_rsp_pkt[1]=0;
	log_rsp_pkt[3]=0;
	log_rsp_pkt[2]=length+12;
	log_rsp_pkt[5]=0;
	log_rsp_pkt[4]=length+12;
	log_rsp_pkt[7]=type>>8;
	log_rsp_pkt[6]=type&0xff;
	log_rsp_pkt[11]=timestamp>>56;
	log_rsp_pkt[10]=timestamp>>48;
	log_rsp_pkt[9]=timestamp>>40;
	log_rsp_pkt[8]=timestamp>>32;
	log_rsp_pkt[13]=timestamp>>24;
	log_rsp_pkt[12]=timestamp>>16;
	log_rsp_pkt[15]=timestamp>>8;
	log_rsp_pkt[14]=timestamp&0xff;
	memcpy(&(log_rsp_pkt[16]),pkt,length);
	
	ret = ftm_diag_encode_send(log_rsp_pkt, len);
	if(debug_op) printf("wrote %d bytes to fd %d\n",ret, qdiag_fd);
	
	return 0;
}

typedef enum
{
  FTM_BT_DRV_NO_ERR = 0,
  FTM_BT_DRV_CONN_TEST_FAILS,
  FTM_BT_DRV_QSOC_POWERUP_FAILS,
  FTM_BT_DRV_RX_PKT_TYPE_NOT_SUPPORTED,
  FTM_BT_DRV_SIO_OPEN_FAILS,
  FTM_BT_DRV_NO_SOC_RSP_TOUT,
  FTM_BT_DRV_BAD_NVM,
#ifdef BT_NV_SUPPORT
  FTM_BT_NV_READ_FAIL,
  FTM_BT_NV_WRITE_FAIL,
#endif
  FTM_BT_DRV_UNKNOWN_ERR
} ftm_bt_drv_err_state_type;
/* FTM status log size*/
const uint8_t logsize = 2;
/* HCI user Cmd pass Log Packet */
const uint8_t event_buf_user_cmd_pass[2] = {0x0f,FTM_BT_DRV_NO_ERR};
/* HCI user Cmd fail Log Packet */
const uint8_t event_buf_user_cmd_fail[2] = {0x0f,FTM_BT_DRV_CONN_TEST_FAILS};
/* HCI user Cmd timed out Log Packet */
const uint8_t event_buf_user_cmd_timeout[2] = {0x0f,FTM_BT_DRV_NO_SOC_RSP_TOUT};
/* HCI user Cmd Unknown error Log Packet */
const uint8_t event_buf_user_unknown_err[2] = {0x0f,FTM_BT_DRV_UNKNOWN_ERR};


/* Command opcode pack/unpack */
#define cmd_opcode_pack(ogf, ocf)	(uint16_t)((ocf & 0x03ff)|(ogf << 10))
#define cmd_opcode_ogf(op)		(op >> 10)
#define cmd_opcode_ocf(op)		(op & 0x03ff)
int ftm_bt_sendcmd
(
  uint8_t  *pBuffer,
  uint8_t  nSize
 )
 {
	 int n=0;
	 ftm_bt_send_log_packet(FTM_BT_CMD_CODE,pBuffer,nSize);
	 n = write(bt_fd,pBuffer, nSize);
	 return n;
 }

uint8_t ftm_bt_hci_hal_vs_sendcmd
(
  uint16_t opcode,
  uint8_t  *pCmdBuffer,
  uint8_t  nSize
)
{
  uint8_t cmd[HC_VS_MAX_CMD_EVENT];   //JN: change this
  uint8_t status = 0;
  uint8_t *msg= (&cmd[0]);
  uint32_t len=HCI_CMD_HDR_SIZE+nSize;

  cmd[0] = BT_HCI_CMD_PKT; // JN: bluetooth header files in linux has a define
                      // HCI_COMMAND_PKT for this but do we want to use
                      // something thats command between QC platforms.
  cmd[0] = (uint8_t)(opcode & 0xFF);
  cmd[1] = (uint8_t)( (opcode>>8) & 0xFF);
  cmd[2] = (uint8_t)(nSize);
	
  if(pCmdBuffer)
	  memcpy(&cmd[HCI_CMD_HDR_SIZE], pCmdBuffer, nSize);

  status = ftm_bt_sendcmd(msg,len);
  if (status < 0)
  {
    printf("Error->Send Header failed : %d\n",status);
    return -1;
  }
  
  ftm_bt_send_log_packet(FTM_BT_CMD_CODE,pCmdBuffer,nSize);
  return status;
}

uint8_t ftm_bt_hci_hal_read_nvm()
{	
  const uint8_t getHWVerRegCmd[4] =  {0x01, 0x1B, 0x01, 0x00};
  return ftm_bt_hci_hal_vs_sendcmd(BT_QSOC_NVM_ACCESS_OPCODE,(uint8_t *)&getHWVerRegCmd[0],4);
}

uint8_t ftm_bt_hci_hal_read_hw_version()
{
  const uint8_t getHWVerRegCmd[6] = {0x0D, 0x34, 0x00, 0x00, 0x8C, 0x04 };
  return ftm_bt_hci_hal_vs_sendcmd(BT_QSOC_EDL_CMD_OPCODE,(uint8_t *)&getHWVerRegCmd[0],6);
}

uint8_t ftm_bt_hci_hal_write_bd_addr(uint8_t *bd_addr)
{  
  return ftm_bt_hci_hal_vs_sendcmd(CMD_WRITE_BD_ADDRESS,bd_addr,6);
}

uint8_t ftm_bt_hci_hal_read_bd_addr()
{
  return ftm_bt_hci_hal_vs_sendcmd(BT_QSOC_READ_BD_ADDR_OPCODE,NULL,0);
}

uint8_t ftm_bt_hci_hal_send_reset_cmd()
{
  const uint8_t   ResetCmd[4] = {0x01, 0x03, 0x0C, 0x00};

  return write(bt_fd,(uint8_t*)(&ResetCmd[0]), 4);
}

uint8_t ftm_bt_hci_hal_read_app_version()
{
  const uint8_t getAppVerCmd[] = {0x06};
  return ftm_bt_hci_hal_vs_sendcmd(BT_QSOC_EDL_CMD_OPCODE,(uint8_t *)&getAppVerCmd[0],1);
}

uint8_t bd_addr[6]={0};
int handle_incomming_packet(uint8_t *pkt, uint8_t length)
{
	uint8_t ogf,ocf;
	uint8_t event_buf_nv_read_response[8]={0};
	if(*pkt ==0x0E)
	{
		ocf=pkt[3];
		ogf=pkt[4];
		if((ocf==OCF_READ_BD_ADDR)&&((ogf>>2)==OGF_INFO_PARAM))
		{
			memcpy(bd_addr,&(pkt[6]),6);
			event_buf_nv_read_response[0] = FTM_BT_CMD_NV_READ;
			event_buf_nv_read_response[1] = NV_BD_ADDR_I;
			event_buf_nv_read_response[7] = bd_addr[5];
			event_buf_nv_read_response[6] = bd_addr[4];
			event_buf_nv_read_response[5] = bd_addr[3];
			event_buf_nv_read_response[4] = bd_addr[2];
			event_buf_nv_read_response[3] = bd_addr[1];
			event_buf_nv_read_response[2] = bd_addr[0];
         
			ftm_bt_send_log_packet(FTM_BT_EVENT_CODE,(uint8_t *)(&event_buf_nv_read_response[1]),sizeof(event_buf_nv_read_response)-1);
		}
	}
	return ftm_bt_send_log_packet(FTM_BT_EVENT_CODE,pkt,length);
}

typedef struct
{
  uint8_t opaque_header[4];
}
diagpkt_subsys_header_type;

/* FTM (BT) PKT Header */
typedef struct
{
  uint16_t cmd_id;            /* command id (required) */
  uint16_t cmd_data_len;      /* request pkt data length, excluding the diag and ftm headers
                             (optional, set to 0 if not used)*/
  uint16_t cmd_rsp_pkt_size;  /* rsp pkt size, size of response pkt if different then req pkt
                             (optional, set to 0 if not used)*/
} ftm_bt_cmd_header_type;

/* Bluetooth FTM packet */
typedef struct
{
  diagpkt_subsys_header_type diag_hdr;
  ftm_bt_cmd_header_type     ftm_hdr;
  uint8_t                    data[];
} ftm_bt_pkt_type;

#define HCI_EVT_HDR_SIZE  3

int qdiag_handle_ftm_bt(void *dc,void * data)
{
	struct mbuf *mbuf = (struct mbuf*)data;
	uint8_t *msg;
	int ret=0;
	int i=0;
	uint8_t len=0;
	uint8_t ind=0;
	
	if(debug_op) printf("ftm_bt %d\n",bt_fd);
	msg = (uint8_t *)mbuf->data;
	len = mbuf->size;	
	ftm_bt_pkt_type *pkt=(ftm_bt_pkt_type *)msg;
	if(debug_op)
	{
		printf("msg = ");
		for(i=0;i<len;printf("0x%x ",msg[i++]));
		printf("\n");
		printf("pkt->diag_hdr = 0x%2X%2X%2X%2X\n",pkt->diag_hdr.opaque_header[0],pkt->diag_hdr.opaque_header[1],pkt->diag_hdr.opaque_header[2],pkt->diag_hdr.opaque_header[3]);
		printf("pkt->ftm_hdr.cmd_id = 0x%X\n",pkt->ftm_hdr.cmd_id);
		printf("pkt->ftm_hdr.cmd_data_len = 0x%X\n",pkt->ftm_hdr.cmd_data_len);
		printf("pkt->ftm_hdr.cmd_data = ");
		for(i=0;i<pkt->ftm_hdr.cmd_data_len;printf("0x%2x ",pkt->data[i++]));
		printf("\n");
	}
	switch(pkt->ftm_hdr.cmd_id)
	{
		case(BT_QSOC_EDL_CMD_CODE):
		{
			/* READ BD ADDRESS */ 
			if(pkt->data[0]==0x0B)
			{
				if(pkt->data[1]==NV_BD_ADDR_I)
				{
					ftm_bt_hci_hal_read_bd_addr();
				}
				break;
			}
			/* WRITE BD ADDRESS */ 
			else if(pkt->data[0]==0x0C)
			{
				if(pkt->data[1]==NV_BD_ADDR_I)
				{					
					if(debug_op) printf("errno = %d %s,%d: \n",errno,__FUNCTION__,__LINE__);					//ftm_bt_hci_hal_write_bd_addr(&(pkt->data[2]));
				}
				break;
			}
			else
			{
				ind =(pkt->data[0]==1)?1:0;
				ret =  ftm_bt_sendcmd((uint8_t*)(&pkt->data[ind]), pkt->ftm_hdr.cmd_data_len-ind);
				if(debug_op) printf("wrote %d bytes to fd %d errno=%d\n",ret, bt_fd,errno);
			}
		}
		break;
	}
	ret = ftm_diag_encode_send(msg, len);
	if(debug_op) printf("wrote %d bytes to fd %d\n",ret, qdiag_fd);
	return 0;
}

void qdiag_get_table(struct qdiag_table **tbl_ptr)
{
	struct qdiag_table *ptr=NULL;
	ptr=(struct qdiag_table *)malloc(sizeof(struct qdiag_table));
	ptr->hdr.num_of_entries=1;
	ptr->table=(struct qdiag_table_entry *)malloc(sizeof(struct qdiag_table_entry));
	memcpy(ptr->table,&entry,sizeof(struct qdiag_table_entry));
	*tbl_ptr=ptr;
	return ;
}
void *ftm_readerthread(void *ptr)
{
  fd_set readfds;
  do
  {
    FD_ZERO(&readfds);
    FD_SET(bt_fd, &readfds);
    if(FD_ISSET(bt_fd,&readfds))
    {
      if(debug_op) printf("Read descriptor set\n");
		int n=0;
		int size;
		int i;
		uint8_t buf[1024];
		{			
			n=read(bt_fd,&(buf[0]),1024);
			size = n;
			if (n>0)
			{
				if(debug_op)
				{
					printf("read %d from bt diag\n",n);
					for(i=0;i<n; printf("0x%x ",buf[i++]));
					printf("\n");
				}
				handle_incomming_packet(&buf[0],size);
			}
			else if(n<0) 
			{
				if(debug_op) printf("Can't read from bt diag\n");
				exit(1);
			}	
		}
		if(debug_op) printf("END wrote %d bytes to fd %d\n",n, qdiag_fd);
    }
  }  while(1);
  if(debug_op) printf("\nReader thread exited\n");
  return 0;
}

int qdiag_set_pipe(int fd)
{
	qdiag_fd = fd;
	printf("qdiag_fd = %d\n",qdiag_fd);
	printf("openning BT_RPMSG_CMD_CHANNEL = %s\n",BT_RPMSG_CMD_CHANNEL);
	bt_fd= open(BT_RPMSG_CMD_CHANNEL,O_RDWR);
	if (bt_fd < 0) {
		if(debug_op) printf("FTM_BT %s,%d: can't open bt rpmsg channel\n",__FUNCTION__,__LINE__);
		exit(1);
	}	
	
	if(debug_op) printf("bt_fd = %d\n",bt_fd);
    /* Creating read thread which listens for various masks & pkt requests */
    pthread_create( &hci_cmd_thread_hdl, NULL, ftm_readerthread, NULL);
	return 0;
}

int qdiag_set_debug_level(int level)
{
	debug_op = 1;//level;
	return 0;
}
