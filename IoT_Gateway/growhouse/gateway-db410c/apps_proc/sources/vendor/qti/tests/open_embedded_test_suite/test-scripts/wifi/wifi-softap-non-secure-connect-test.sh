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
#non secure connection to Wifi softap
#creates softap on board that transmits the wifi AP. uses another board to connect to the wifi AP
# arg1 - channel number to bring up softap with. if no channel is given defaults to 6


########################################### function definition ############################################################
FUNC_createHostapConf ()
{
	HOSTAPD_CONF="
interface=$CURR_WLAN
ctrl_interface=/var/run/hostapd
driver=nl80211
hw_mode=g
channel=$SOFTAP_CHANNEL
ssid=$SOFTAP_SSID

#for high throughput 20Mhz
country_code=JP
#advertises the country_code and the set of allowed channels and transmit power levels based on the regulatory limits
ieee80211d=1

ieee80211n=1
wmm_enabled=1
"
	echo "=================> HOSTAPD_CONF=$HOSTAPD_CONF"
}

FUNC_createWpa_suppConf ()
{
	WPA_CONF="ctrl_interface=/run/wpa_supplicant
update_config=1

network={
        ssid=\"$SOFTAP_SSID\"
        key_mgmt=NONE
}
"
	echo "===========> WPA_CONF=$WPA_CONF"
}

FUNC_define_channel ()
{
	if [ "$CHANNEL_FLAG" != "-c" ]; then
		SOFTAP_CHANNEL=6
	else 	#if we get channel flag then, check we get the channel number
		$UTIL_PATH/check-args-num.sh $# 2
	fi
}
############################################## test execution #############################################################
export PATH=/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin:$PATH
########### environment variables from get-env-var-val-target.sh file ######

# defines general variables used in most tests
source $(dirname $(readlink -f $0))/../../util/generally-used-variables.txt

# no checking arguments number because suppling arguments is optional
CHANNEL_FLAG=$1
SOFTAP_CHANNEL=$2

FUNC_define_channel
echo "=================> starting wifi-softap-non-secure-connect-test.sh <========================="
SOFTAP_SSID="softapTestNonSecure"

FUNC_createHostapConf
FUNC_createWpa_suppConf

$UTIL_PATH/connect-helperSTA-to-DUTsoftap.sh "$WPA_CONF" "$HOSTAPD_CONF" || (echo "utility  failed! - connect-helperSTA-to-DUTsoftapV2.sh" && exit 1) || exit 1

$UTIL_PATH/disconnect-helperSTA-from-DUTsoftap.sh

echo "===============> successfully finished wifi-softap-non-secure-connect-test.sh script <===================="
exit 0
























