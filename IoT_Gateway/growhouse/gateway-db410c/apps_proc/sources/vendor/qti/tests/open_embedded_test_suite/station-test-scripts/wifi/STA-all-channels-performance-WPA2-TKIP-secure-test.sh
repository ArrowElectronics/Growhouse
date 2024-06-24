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


#!/bin/bash
# connects DUT Wifi STA to working stations wifi dongle in every channel and runs iperf


#=====================> function declaration  <===========================
configureHostapd () #$1 - channel number
{
	HOSTAPD_CONF="
interface=$CURR_WLAN
ctrl_interface=/var/run/hostapd
driver=nl80211
hw_mode=g
channel=$1
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
wpa_pairwise=TKIP CCMP
wpa_passphrase=$SOFTAP_PASS
rsn_pairwise=CCMP
macaddr_acl=0
"


}

configureDhcpd ()
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
$SOFTAP_GATEWAY;
# --- Netmask
option subnet-mask
255.255.255.0;
# --- Broadcast Address
option broadcast-address
192.168.8.255;
# --- Domain name servers, tells the clients which DNS servers to use.
option domain-name-servers
$SOFTAP_GATEWAY, 8.8.8.8, 8.8.4.4;
option time-offset
0;
range 192.168.8.3 192.168.8.128;
default-lease-time 1209600;
max-lease-time 1814400;
}
"
}

cleanup ()
{
	echo "===> cleanup - killing wpa_supplicant, hostapd, dhcpd and removing log files <======"
	ssh $TARGET_ADDR $DEVICE_UTIL_PATH/kill-process-ids.sh wpa_supplicant
	$UTIL_PATH/kill-process-ids.sh hostapd
	$UTIL_PATH/kill-process-ids.sh dhcpd
	rm $WS_IPERF_LOG &>/dev/null
	rm $DUT_IPERF_LOG &>/dev/null
	sleep 5
}

#=====================> script execution  <===========================
echo "===================> starting STA-all-channels-performance-WPA2-TKIP-secure-test.sh <===================="

# defines general variables used in most tests
source $(dirname $(readlink -f $0))/../../util/generally-used-variables.txt

SOFTAP_SSID="STA-all-channel"
SOFTAP_PASS="12345678"
SOFTAP_GATEWAY="192.168.8.1"

WS_IPERF_LOG="$CURR_PATH/ws-iperf.log"
DUT_IPERF_LOG="$CURR_PATH/dut-iperf.log"
IPERF_RUNNING_TIME=35

cleanup
configureDhcpd

for CHANNEL in $(seq 1 13)
do
# run in loop for channel 1-13

	# bring up softap in channel num N
	configureHostapd $CHANNEL

	echo "=========== testing hostapd conf file ================="
	echo $HOSTAPD_CONF
	echo "=========== testing dhcpd conf file ================="
	echo $DHCPD_CONF
	$UTIL_PATH/bringUp-softap.sh "$HOSTAPD_CONF" "$DHCPD_CONF" \
	|| (echo "==> bring-up softap utility failed <== in channel $CHANNEL. so far iperf logs:$ALL_CHANNEL_RES" && exit 1) \
	|| exit 1

	echo "====> connect DUT to sofap <======="
	ssh $TARGET_ADDR $WIFI_TEST_PATH/wifi-connect-WPA2-PSK-security-test.sh $SOFTAP_SSID $SOFTAP_PASS \
	|| (echo "DUT failed to connect to wifi in channel $CHANNEL. so far iperf logs:$ALL_CHANNEL_RES" && exit 1) \
	|| exit 1
	sleep 5

	echo "=======> run iperf for $IPERF_RUNNING_TIME seconds  DUT-client	WS-server <============="
	iperf -s 1> $WS_IPERF_LOG &
	sleep 5

	echo "$( (ssh $TARGET_ADDR iperf -c $SOFTAP_GATEWAY -i 1 -t $IPERF_RUNNING_TIME) 2>&1)" > $DUT_IPERF_LOG
	cat $DUT_IPERF_LOG | grep "connect failed" \
	&& echo "iperf -c from DUT failed in channel $CHANNEL. so far iperf logs:$ALL_CHANNEL_RES" \
	&& exit 1
	
	echo "=====> pars iperf results <======"
	DUT_IPERF_RES=$(awk 'NR==1; END{print}' $DUT_IPERF_LOG | rev | cut -d" " -f2 | rev | grep -e "---" -v)
	
	echo "====> DUT_IPERF_RES=$DUT_IPERF_RES in CHANNEL=$CHANNEL <============"
	sleep 3
	echo "===> kill iperf server on WS <====="
	$UTIL_PATH/kill-process-ids.sh iperf
	sleep 1
	$UTIL_PATH/kill-process-ids.sh iperf
	sleep 2

	echo "=====> run iperf  for $IPERF_RUNNING_TIME seconds   WS-client	DUT-server <================"
	echo "$( (ssh $TARGET_ADDR iperf -s) 2>&1)" > $DUT_IPERF_LOG &
	sleep 1
	
	echo "====> get DUT ip address <======"
	DUT_WLAN="$(ssh $TARGET_ADDR ifconfig -a | grep wlan | cut -d" " -f1)"
	DUT_IP_ADDR="$(ssh $TARGET_ADDR ifconfig | grep -A 1 $DUT_WLAN | grep "inet addr" | cut -d":" -f2 | cut -d" " -f1)"
	iperf -c "$DUT_IP_ADDR" -i 1 -t "$IPERF_RUNNING_TIME" &>"$WS_IPERF_LOG"
	cat $WS_IPERF_LOG | grep "connect failed" \
	&& echo "iperf -c from WS failed in channel $CHANNEL. so far iperf logs:$ALL_CHANNEL_RES" \
	&& exit 1

	echo "====> parse iperf results <======"
	WS_IPERF_RES=$(awk 'NR==1; END{print}' $WS_IPERF_LOG | rev | cut -d" " -f2 | rev | grep -e "---" -v)
	
	echo "====> WS_IPERF_RES=$WS_IPERF_RES in CHANNEL=$CHANNEL <============"
	sleep 3
	echo "=====> kill iperf server on DUT <======="	
	ssh $TARGET_ADDR $DEVICE_UTIL_PATH/kill-process-ids.sh iperf
	sleep 1
	ssh $TARGET_ADDR $DEVICE_UTIL_PATH/kill-process-ids.sh iperf
	

	echo "====> concatenate results for current channel $CHANNEL<====="
	CURR_CHANNEL_RES="channel number $CHANNEL\nDUT client - $DUT_IPERF_RES\nWS client - $WS_IPERF_RES\n"
	ALL_CHANNEL_RES="$ALL_CHANNEL_RES$CURR_CHANNEL_RES"

	# cleanup
	cleanup
	sleep 5
done

echo "==========>  final result: <=============="
echo -e "$ALL_CHANNEL_RES"
echo "==============> successfully ended STA-all-channels-performance-WPA2-TKIP-secure-test.sh <============================"
















