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
#brings up softap and dhcp server
#receives as args: $1 - softap(hostapd) configuration    $2 - dhcp server (dhcpd) configuration



########################################### function definition ############################################################
FUNC_createConf () # $1 - configuration file path $2 - configuration content
{
	echo "FUNC_createConf arg1 conf path  = $1"
	echo "FUNC_createConf arg2 conf content = $2"
	
	echo "$2" > $1 
	echo "========> for debug. configuration of file :$1 are:"
	cat $1
}

FUNC_Cleanup ()
{
	$UTIL_PATH/kill-process-ids.sh wpa_supplicant
	$UTIL_PATH/kill-process-ids.sh hostapd
	$UTIL_PATH/kill-process-ids.sh dhcpd
	rfkill unblock all

	rm $HOSTAPD_CONF_PATH 2>/dev/null
	rm $DHCPD_CONF_PATH 2>/dev/null
	rm $HOSTAPD_RET_VAL_LOG 2>/dev/null
	rm $DHCPD_RET_VAL_LOG 2>/dev/null

	ifconfig $CURR_WLAN down
	sleep 3
	ifconfig $CURR_WLAN up
}

FUNC_parseDhcpdConf () #$1 - conf file path  $2 - softap static ip  $3 - softap netmask
{
	SOFTAP_NETMASK=$(awk '/netmask/ {print $3}' $DHCPD_CONF_PATH)
	SOFTAP_STATIC_IP=$(awk 'count && !--count; /option routers/ {count=1}' $DHCPD_CONF_PATH | cut -d";" -f1) || (echo "cut failed" && exit 1) || exit 1

	echo "==================> in function(FUNC_parseDhcpdConf)SOFTAP_NETMASK=$SOFTAP_NETMASK"
	echo "==================> in function(FUNC_parseDhcpdConf)SOFTAP_STATIC_IP=$SOFTAP_STATIC_IP"
}

FUNC_run_command_multipule_attempts () # $1 - command to execute  $2 - attempts num   $3 - time between attempts 
{
	local COMMAND="$1"
	local ATTEMPTS=$2
	local TIME=$3
	local COUNT=0
	local RESULT=0

	while [ $COUNT -lt $ATTEMPTS ]
	do
		echo "====> giving time for hostapd enablment - sleeping 30 sec <====="
		sleep $TIME
		echo "==========> COMMAND=$COMMAND <==========="
		eval $COMMAND
		RESULT=$?
		echo "==========> RESULT=$RESULT <==========="	
		if [ $RESULT -eq 0 ]; then
			break
		fi
		
		COUNT=$((COUNT=COUNT+1))
	done

	return $RESULT
}

exit_cleanup_and_posting () # $1 - string to echo
{
	EXIT_STATMENT="$1"
	
	echo 

}

############################################## test execution #############################################################
######### arguments ##############
HOSTAPD_CONF=$1		#$1 - softap configuration 
DHCPD_CONF=$2		#$2 - dhcp server (dhcpd) configuration

export PATH=/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin:$PATH
########### environment variables from get-env-var-val-target.sh file ########################
# defines general variables used in most tests
source "$(dirname $(readlink -f $0))/generally-used-variables.txt"

#UTIL_PATH="$(dirname $(readlink -f $0))"



echo "===========> starting bring up softap utility   <========================="
echo "====> arguments are: arg1 - hostapd configuration. arg2 - dhcpd configuration"
echo "====> arg1 HOSTAPD_CONF=$HOSTAPD_CONF"
echo "====> arg2 DHCPD_CONF=$DHCPD_CONF"

echo "===========> cleaning up before setup  <========================="
HOSTAPD_CONF_PATH=/etc/hostap.conf
DHCPD_CONF_PATH=/etc/dhcp/dhcpd-test.conf
HOSTAPD_RET_VAL_LOG=hostapd_ret_val.log
DHCPD_RET_VAL_LOG=dhcpd_ret_val.log
FUNC_Cleanup
echo "===========> give time (5 sec) for killed processes to release resources <============"
sleep 5

echo "===========> verifying wifi driver supports softap interface mode <============"
RESULT=$(iw list | grep "Supported interface modes" -A 10 | grep "* AP$" ) \
|| (echo "===> wifi driver do not support AP mode <==" && exit 1) || exit 1

echo "===========> creating configuration files <========================="
FUNC_createConf "$HOSTAPD_CONF_PATH" "$HOSTAPD_CONF"
FUNC_createConf "$DHCPD_CONF_PATH" "$DHCPD_CONF"
sleep 5
echo "===========> starting HOSTAPD <========================="
hostapd -d $HOSTAPD_CONF_PATH >> $HOSTAPD_RET_VAL_LOG &
FUNC_run_command_multipule_attempts 'cat $HOSTAPD_RET_VAL_LOG | grep "$CURR_WLAN: AP-ENABLED"' 5 30 \
|| (echo "====> hostapd failed! <====" && exit 1) || exit 1

echo "===========> starting DHCP server  <========================="
FUNC_parseDhcpdConf || (echo "FUNC_parseDhcpdConf failed" && exit 1) || exit 1
echo "==================> SOFTAP_NETMASK=$SOFTAP_NETMASK"
echo "==================> SOFTAP_STATIC_IP=$SOFTAP_STATIC_IP"
ifconfig $CURR_WLAN up $SOFTAP_STATIC_IP netmask $SOFTAP_NETMASK
sleep 4
dhcpd -d -cf $DHCPD_CONF_PATH $CURR_WLAN 2>> $DHCPD_RET_VAL_LOG &
sleep 3
grep "Server starting service" $DHCPD_RET_VAL_LOG || (echo "====> dhcpd failed! <=====" && exit 1) || exit 1

############################## end of execution. now cleanup #################################################

rm $HOSTAPD_CONF_PATH 2>/dev/null
rm $DHCPD_CONF_PATH 2>/dev/null
#rm $HOSTAPD_RET_VAL_LOG 2>/dev/null
rm $DHCPD_RET_VAL_LOG 2>/dev/null

sleep 5
echo "===========> bring up softap utility successfully ended  <========================="





