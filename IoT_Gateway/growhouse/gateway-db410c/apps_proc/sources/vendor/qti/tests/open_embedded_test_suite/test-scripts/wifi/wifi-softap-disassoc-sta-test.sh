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
#testing softap functionality - removing a connected station
#scripts connects HELPER board(DB410) to DUT's softap, then from softap disassociates HELPER board.
#receives no arguments


########################################### function definition ############################################################
FUNC_createHostapConf ()
{
	HOSTAPD_CONF="
interface=$CURR_WLAN
ctrl_interface=/var/run/hostapd
driver=nl80211
hw_mode=g
channel=6
ssid=$SOFTAP_SSID

#for high throughput 20Mhz
country_code=JP
#advertises the country_code and the set of allowed channels and transmit power levels based on the regulatory limits
ieee80211d=1

ieee80211n=1
wmm_enabled=1

auth_algs=1
wpa=2
wpa_key_mgmt=WPA-PSK
wpa_passphrase=$SOFTAP_PASS
rsn_pairwise=CCMP
macaddr_acl=0
"
	echo "=================> HOSTAPD_CONF=$HOSTAPD_CONF"
}

FUNC_createWpa_suppConf ()
{
	WPA_CONF="ctrl_interface=/run/wpa_supplicant
update_config=1

network={
        ssid=\"$SOFTAP_SSID\"
        psk=\"$SOFTAP_PASS\"
}
"
	echo "===========> WPA_CONF=$WPA_CONF"
}

FUNC_getConnect_time () #if fails to get connection time echos 0
{
	HELPER_MAC=$(FUNC_getHelperMacFromHelper)
	CONNECTED_TIME=$(hostapd_cli all_sta | grep -io "$HELPER_MAC" -A 11 | grep "connected_time")
	echo $CONNECTED_TIME | cut -d"=" -f2
}
############################################## test execution #############################################################
export PATH=/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin:$PATH
########### environment variables from get-env-var-val-target.sh file ######

# defines general variables used in most tests
source $(dirname $(readlink -f $0))/../../util/generally-used-variables.txt
# defines common used functions in softap tests scripts
source $UTIL_PATH/softap-HELPER-funcs.sh

echo "=================> starting wifi-softap-disassoc-sta-test.sh <========================="
SOFTAP_SSID="softapTestDisassocSTA"
SOFTAP_PASS="23456789"

FUNC_createHostapConf
FUNC_createWpa_suppConf
######################## connecting two DB410 via wifi STA to softap ##################################
$UTIL_PATH/connect-helperSTA-to-DUTsoftap.sh "$WPA_CONF" "$HOSTAPD_CONF" || (echo "utility  failed! - connect-helperSTA-to-DUTsoftap.sh" && exit 1) || exit 1
########################################## testing ###################################################
sleep 5
TEST_RESULT=1
echo "test result before=$TEST_RESULT"
#connected time before - all_sta - get mac addres connected time
CON_TIME_BEFORE=$(FUNC_getConnect_time) || (echo "FUNC_getConnect_time failed! before hostapd_cli disassociate. CON_TIME_BEFORE=$CON_TIME_BEFORE" && exit 1) || exit 1

hostapd_cli disassociate $(FUNC_getHelperMacFromDUT)
sleep 15

#connected time after all_sta - get mac addres connected time
CON_TIME_AFTER=$(FUNC_getConnect_time) || (echo "FUNC_getConnect_time failed! after hostapd_cli disassociate. CON_TIME_AFTER=$CON_TIME_AFTER" && exit 1) || exit 1

echo "CON_TIME_AFTER=$CON_TIME_AFTER"
if [ $CON_TIME_BEFORE -gt $CON_TIME_AFTER ]; then
	echo "inside if statement: $CON_TIME_BEFORE is bigger than $CON_TIME_AFTER"
	TEST_RESULT=0
fi
echo "test result after=$TEST_RESULT"
################################ disconnecting ########################################
$UTIL_PATH/disconnect-helperSTA-from-DUTsoftap.sh 
(exit $TEST_RESULT ) || (echo "==> failed wifi-softap-disassoc-sta-test.sh <===" && exit 1) || exit 1

echo "===============> successfully finished wifi-softap-disassoc-sta-test.sh <===================="
exit 0






















