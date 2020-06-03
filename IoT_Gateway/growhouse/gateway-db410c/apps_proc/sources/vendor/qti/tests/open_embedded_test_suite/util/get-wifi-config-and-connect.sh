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
#util script for wifi connection test-scripts
#receives configuration info to connect to wifi AP
# $1 configuration info


############################ functions definition ####################################################################
createWpaConf () # $1 conf information. $2 = wpa_supplicant.conf path 
{
	echo "$1" > $2
}

runCmdCheckAndLog () # $1 command to execute and check. $2 expected ret val if successful. $3 logg msg. $4 sleep time between attempts. $5 num of attempts. $6 - expected ret val if failed
{
	ATTEMPTS=$5
	COUNTER=0
	SUCCESS_PHRASE=$2
	FAILE_PHRASE=$6

	echo "BEFORE WHILE LOOOP: ATTEMPTS=$ATTEMPTS => COUNTER=$COUNTER"

	while [ $COUNTER -lt $ATTEMPTS ]; do
		COUNTER=$((COUNTER+1))
		echo ""
		echo "IN WHILE LOOP EXECUTING=$1"

		STDOUT_RET_VAL="$( ($1) 2>&1)"

		sleep $4
		
		echo "STDOUT_RET_VAL=$STDOUT_RET_VAL"
		echo "SLEEPS AMOUNT OF SECONDS=$4"		
		echo "ATTEMPTS=$ATTEMPTS => COUNTER=$COUNTER"

		RESULT=$(echo "$STDOUT_RET_VAL" | grep "$FAILE_PHRASE")
		RETVAL=$?
		echo "FAILE_PHRASE RETVAL=$RETVAL"
		if [ $RETVAL -eq 0 ]; then
			continue
		fi

		RESULT=$(echo "$STDOUT_RET_VAL" | grep "$SUCCESS_PHRASE")
		RETVAL=$?
		echo "SUCCESS_PHRASE RETVAL=$RETVAL"
		if [ $RETVAL -eq 0 ]; then
			break
		fi

	done
	
	RESULT=$(echo "$STDOUT_RET_VAL" | grep "$2") || 
	( ($UTIL_PATH/logger.sh $THIS_FILE_NAME $LOG -v "$3. STDERR:$STDOUT_RET_VAL" 51) && (echo "$3. STDERR:$STDOUT_RET_VAL") && (exit 1)) || exit 1
}

getIpFromUdhcp ()
{
	echo "IN FUNCTION: getIpFromUdhcp"

	runCmdCheckAndLog "udhcpc -i $CURR_WLAN" "obtained" "udhcpc shulden't fail" 4 4 "ip: not found"
	
	runCmdCheckAndLog "ifconfig $CURR_WLAN" "inet addr" "udhcpc failed to obtain IP for wifi AP" 3 2 "DUMMY expected error"
}

pingGatewayOfConnectedWifiAp ()
{
	LINE_CONTAINING_GATEWAY_IP=$(ip route show | grep $CURR_WLAN | grep default) && echo "LINE_CONTAINING_GATEWAY_IP=$LINE_CONTAINING_GATEWAY_IP" || 
	( ($UTIL_PATH/logger.sh $THIS_FILE_NAME $LOG -v "failed to grep gateway ip" 58) && (echo "failed to grep gateway ip") &&  (exit 1) ) || exit 1

	for ITEM in $LINE_CONTAINING_GATEWAY_IP; do
		if [ $ITEM != "dev" ]; then
			PREV=$ITEM
		else
			GATEWAY_IP=$PREV
		fi
	done

	echo "GATEWAY_IP=$GATEWAY_IP"

	runCmdCheckAndLog "ping -c 1 $GATEWAY_IP" "received, 0% packet loss" "faild to ping gateway IP" 5 7 "0 packets received" || exit 1
}
############################## test run #####################################################################################
echo "==================>  starting  get-wifi-config-and-connect.sh <======================================="
export PATH="/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin:/sbin/ip":$PATH

# defines general variables used in most tests
source "$(dirname $(readlink -f $0))/generally-used-variables.txt"

CONF_INFO="$1"

THIS_FILE_NAME=$0
LOG=-2
WPA_SUPP_CONF_PATH=/etc/wpa_supplicant_test.conf

$UTIL_PATH/kill-process-ids.sh wpa_supplicant
$UTIL_PATH/kill-process-ids.sh udhcpc


createWpaConf "$CONF_INFO" $WPA_SUPP_CONF_PATH 
echo "=======> sole argument of script: wpa_supplicant configuration: <==================="
echo "CONF_INFO: $CONF_INFO"

rfkill unblock all

runCmdCheckAndLog "wpa_supplicant -B -i $CURR_WLAN -c $WPA_SUPP_CONF_PATH" "Successfully initialized wpa_supplicant" "wpa_supplicant configuration failed" 1 2 "nl80211:" || exit 1

runCmdCheckAndLog "wpa_cli status" "wpa_state=COMPLETED" "wpa_cli status - dose not show:\"wpa_state=COMPLETED\"" 6 20 "wpa_state=DISCONNECTED" || exit 1

getIpFromUdhcp || exit 1

pingGatewayOfConnectedWifiAp || exit 1

rm $WPA_SUPP_CONF_PATH

















