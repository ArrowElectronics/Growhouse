#!/bin/sh
#Copyright (c) 2017, The Linux Foundation. All rights reserved.
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

############################### description ###################################
## this scripts connects DUT to HELPER via Bluetooth over a2dp protocol to allow playing of 
## audio from DUT on HELPER.
## for this test HELPER needs to be connected with USB headset
###############################################################################

######################### script execution ######################################################
CURR_DIR_PATH="$(dirname $(readlink -f $0))/"
. "$CURR_DIR_PATH"/../../../lib/sh-test-lib

#TODO take out set-result-dir.sh from lib. create a new directory in lib named "OE-tests-lib"
. "$CURR_DIR_PATH"/../../../lib/set-result-dir.sh

# general BT tests variables
. "$CURR_DIR_PATH"/../BT-tests-config.sh
. "$CURR_DIR_PATH"/../BT-sh-lib.sh

#../ssh-authenticate.sh $HELPER_IP_ADDR

## close_BT function defined in BT-sh-lib.sh
#trap close_BT EXIT

##bring up DUT
#change_BT_status "up"

##bring up HELPER
#change_BT_status "up" $HELPER_IP_ADDR

#make_discoverable "HELPER"
#sleep 10

#TODO see if can define pulse audio permissions in the build out of the box without the need to 
# define it an rebot for each board

enable_pulse_audio

#../util/bluetoothctl_A2DP_automation_SPEAKER.sh
#set_USB_speaker_as_default_audio_in_HELPER.sh

#util/set_USB_speaker_as_default_audio_in_HELPER.sh
#check_return "set_USB_speaker_as_default_audio_in_HELPER"


#bluetoothctl_A2DP_automation_HELPER() {
#	util/HELPER_confirm_DUT_connection_request.sh
#	check_return "HELPER_confirm_DUT_connection_request"
#}


# these argument are set in BT-sh-lib.sh and in BT-tests-config.sh
#util/pairing_from_DUT_to_HELPER.sh $HELPER_MAC_ADDR $DEBUG_OPTION $OUTPUT_OPTION

#bluetoothctl_A2DP_automation() {
#	#../util/bluetoothctl_A2DP_automation.sh
#	#check_return "BT-A2DP-test"
#	util/BT_connect_and_a2dp_sink_DUT_to_HELPER.sh $HELPER_MAC_ADDR
#}

#bluetoothctl_A2DP_automation_HELPER &
#bluetoothctl_A2DP_automation &

echo "===== starting expect script $CURR_DIR_PATH/util/set_USB_speaker_as_default_audio_in_HELPER.sh=========="
run_and_check "$CURR_DIR_PATH/util/set_USB_speaker_as_default_audio_in_HELPER.sh"
echo "===== starting expect script $CURR_DIR_PATH/util/HELPER_confirm_DUT_connection_request.sh =========="
run_and_check "$CURR_DIR_PATH/util/HELPER_confirm_DUT_connection_request.sh" &
echo "===== starting expect script $CURR_DIR_PATH/util/BT_connect_and_a2dp_sink_DUT_to_HELPER.sh $HELPER_MAC_ADDR =========="
run_and_check "$CURR_DIR_PATH/util/BT_connect_and_a2dp_sink_DUT_to_HELPER.sh $HELPER_MAC_ADDR" &

