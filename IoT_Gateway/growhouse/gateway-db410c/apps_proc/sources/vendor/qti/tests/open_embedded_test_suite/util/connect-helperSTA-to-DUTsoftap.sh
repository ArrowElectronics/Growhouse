#Copyright (c) 2016, The Linux Foundation. All rights reserved.
#
#Redistribution and use in source and binary forms, with or without
#modification, are permitted provided that the following conditions are
#met:
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above
#      copyright notice, this list of conditions and the following
#      disclaimer in the documentation and/or other materials provided
#      with the distribution.
#    * Neither the name of The Linux Foundation nor the names of its
#      contributors may be used to endorse or promote products derived
#      from this software without specific prior written permission.
#
#THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
#WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
#MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
#ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
#BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
#BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
#OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
#IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#!/bin/sh
#creates softap on board that transmits the wifi AP. uses another board to connect to the wifi AP
#terminology: 
#DUT = board that creates a softap 
#HELPER = board that connects to the wifi AP

#receives as args: $1 - wpa_supplicant configuration ,	$2 - hostapd configuration


########################################### function definition ############################################################
FUNC_createDhcpdConf () #$1 - softap static ip   $2 - softap netmask
{
	DHCPD_CONF="
ddns-update-style none;
ignore client-updates;
authoritative;
option local-wpad code 252 = text;
 
subnet
192.168.8.0 netmask 255.255.255.0 {
# --- default gateway
option routers
192.168.8.1;
# --- Netmask
option subnet-mask
255.255.255.0;
# --- Broadcast Address
option broadcast-address
192.168.8.255;
# --- Domain name servers, tells the clients which DNS servers to use.
option domain-name-servers
192.168.8.1, 8.8.8.8, 8.8.4.4;
option time-offset
0;
range 192.168.8.3 192.168.8.128;
default-lease-time 1209600;
max-lease-time 1814400;
}
"
}

FUNC_Cleanup ()
{
	echo "doing cleanup"
$UTIL_PATH/kill-process-ids.sh wpa_supplicant
$UTIL_PATH/kill-process-ids.sh hostapd
$UTIL_PATH/kill-process-ids.sh dhcpd
#	/usr/sbin/rfkill unblock all	
	rfkill unblock all
}

FUNC_movingFilesToClient ()
{
	ssh $HELPER_ADDR mkdir $BASE_PATH
	ssh $HELPER_ADDR mkdir $TEST_SCRIPTS_PATH
	ssh $HELPER_ADDR mkdir $DEVICE_UTIL_PATH
	scp $UTIL_PATH/* $HELPER_ADDR:$DEVICE_UTIL_PATH
}

############################################## test execution #############################################################
export PATH=/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin:$PATH
########### environment variables from get-env-var-val-target.sh file ##

# defines general variables used in most tests
source "$(dirname $(readlink -f $0))/generally-used-variables.txt"

THIS_FILE_NAME=$0
FAIL=1
LOG=3
######### arguments ##############
echo "===============> starting utility: connect-helperSTA-to-DUTsoftapV2.sh <============================"
WPA_SUPP_CONF="$1"
echo "==========================> argument 1 WPA_SUPP_CONF=$WPA_SUPP_CONF"
HOSTAPD_CONF="$2"
echo "==========================> argument 2 HOSTAPD_CONF=$HOSTAPD_CONF"

FUNC_Cleanup
sleep 1

FUNC_createDhcpdConf
$UTIL_PATH/bringUp-softap.sh "$HOSTAPD_CONF" "$DHCPD_CONF" || (echo "==> bringup softap util failed <==" && exit 1) || exit 1

echo "===============> transferring wifi connection scripts to CLIENT device <============================"
FUNC_movingFilesToClient
echo "=================> connecting from CLIENT <================================="
STDERR="$( (ssh $HELPER_ADDR "$DEVICE_UTIL_PATH/get-wifi-config-and-connect.sh '$WPA_SUPP_CONF'") 2>&1)" \
|| (echo "STDERR=$STDERR. failed to connect to softap" && exit 1) || exit 
echo "STDERR=$STDERR"
sleep 5
echo "===============> successfully finished connect-helperSTA-to-DUTsoftapV2.sh <============================"







