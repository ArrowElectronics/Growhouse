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
# tests turns on and of user leds  


# defines general variables used in most tests
source $(dirname $(readlink -f $0))/../../util/generally-used-variables.txt

TIME=$1

$UTIL_PATH/check-args-num.sh $# 1 || ( echo "wrong number of arguments" && exit 1 ) || exit 1

LED_MSG_TO_USER="$CONTENT_PATH/$($GET_ENV_VARS LED_TEST_MSG_TO_USER)"

echo "==== testing LED_MSG_TO_USER=$LED_MSG_TO_USER ===="

echo "=============> Running Test Test: leds_on/off <=============="
echo  
date
aplay $LED_MSG_TO_USER -D 'plughw:0,0'
$UTIL_PATH/led_on-off.sh $TIME 1
$UTIL_PATH/led_on-off.sh $TIME 2
$UTIL_PATH/led_on-off.sh $TIME 3
$UTIL_PATH/led_on-off.sh $TIME 4

echo "=============> Finished Test:  leds_on/off <=============="
echo  
date
