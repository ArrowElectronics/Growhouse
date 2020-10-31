/*
 * Copyright (c) 2016, The Linux Foundation. All rights reserved.
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

#ifndef __GPS_PROXY_H__
#define __GPS_PROXY_H__

#define QMI_LOC_NMEA_STRING_MAX_LENGTH_V02 201

enum QGPS_IOC_TTY_IOCTL_CMDS {
	QGPS_REGISTER_HANDLE_IOC = 0,
	QGPS_SEND_NMEA_IOC,
	QGPS_IS_ACTIVE_IOC,
};
/*#define QGPS_REGISTER_HANDLE 0
#define QGPS_SEND_NMEA 1
#define QGPS_IS_ACTIVE 2*/

#define QGPS_IOC_MAGIC 'q'
#define QGPS_REGISTER_HANDLE _IO(QGPS_IOC_MAGIC, QGPS_REGISTER_HANDLE_IOC)
#define QGPS_SEND_NMEA _IO(QGPS_IOC_MAGIC, QGPS_SEND_NMEA_IOC)
#define QGPS_IS_ACTIVE _IO(QGPS_IOC_MAGIC, QGPS_IS_ACTIVE_IOC)

struct gps_proxy_data {
	size_t nmea_length;
	char nmea_string[QMI_LOC_NMEA_STRING_MAX_LENGTH_V02];
};

#endif /* __GPS_PROXY_H__ */
