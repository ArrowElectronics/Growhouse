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
#terminates Wifi connection correctly between helper-DB410 from DUT's softap
#receives no arguments 
#terminology: 
#HELPER = board that connects to the wifi AP created by the DUT



########### environment variables from get-env-var-val-target.sh file ##
export PATH=/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin:$PATH
# defines general variables used in most tests
source "$(dirname $(readlink -f $0))/generally-used-variables.txt"

# defines common used functions in softap tests scripts
source $UTIL_PATH/softap-HELPER-funcs.sh


echo "============> starting disconnect-helperSTA-from-DUTsoftap.sh  <========================"

echo "ssh command remove client test jobs"
ssh $HELPER_ADDR rm -r $TEST_SCRIPTS_PATH 

echo "ssh command remove client util path"
ssh $HELPER_ADDR rm -r $DEVICE_UTIL_PATH 

echo "ssh command wpa_cli disconnect"
ssh $HELPER_ADDR 'wpa_cli disconnect'

echo "ssh command kill wpa_supplicant"
ssh $HELPER_ADDR 'kill -9 $(pidof wpa_supplicant)'

FUNC_checkHELPERHasOnlyOneWlan
HELPER_WLAN="$(ssh $HELPER_ADDR ifconfig -a | grep wlan | cut -d" " -f1)"
echo "ssh command dhclient $HELPER_WLAN -r"
ssh $HELPER_ADDR dhclient $HELPER_WLAN -r

echo "ssh command $HELPER_WLAN down"
ssh $HELPER_ADDR ifconfig $HELPER_WLAN down
sleep 2

echo "ssh command $HELPER_WLAN up"
ssh $HELPER_ADDR ifconfig $HELPER_WLAN up

echo "DUT command kill hostapd"
kill -9 $(pidof hostapd)
sleep 1

echo "DUT command kill dhcpd"
kill -9 $(pidof dhcpd)
sleep 1

echo "DUT command $CURR_WLAN down"
ifconfig $CURR_WLAN down
sleep 3

echo "DUT command $CURR_WLAN up"
ifconfig $CURR_WLAN up
sleep 3


echo "============> finished disconnect-helperSTA-from-DUTsoftap.sh  <========================"




