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


###############################################################################
### description: this file contains functions to be used through bt test scripts
### this script should be run with dot "." e.g. ". scriptName.sh"
### BT-tests-config.sh file must be run before this script. 
### e.g. ". <path to file>/BT-tests-config.sh"
###############################################################################

###############################################################################
### extracting HELPER MAC address - this is a global variable for all BT tests
###############################################################################
HELPER_MAC_ADDR="$(ssh $SSH_PARAMS root@$HELPER_IP_ADDR "hciconfig -a | grep -o '..:..:..:..:..:..'")"
DUT_MAC_ADDR="$(hciconfig -a | grep -o '..:..:..:..:..:..')"

###############################################################################
### description: function brings up or closes BT device
### up - unblocks RF, turns on BT device, enables BT scan and makes device discoverable
### down - reverses operations that were run in "up" to previous state
### arg1(mandatory) - expecting string "up" or "down"
### arg2(optional) - host IP
###############################################################################
change_BT_status()
{
	local status=$1
	local hostIP=$2
	
	# setting if it's up or down
	if [ $status = "up" ]; then
		local block_status="unblock"
		local hci_status="up"
	elif [ $status = "down" ]; then
		local block_status="block"
		local hci_status="down"
	else
		error_msg "wrong parameter given to function change_BT_status"
	fi
	
	local rfkill_command="rfkill $block_status bluetooth"
	local hci0_command="hciconfig hci0 $hci_status"

	# setting if it's DUT or HELPER
	if [ -z $hostIP ]; then
		eval $rfkill_command
		exit_on_fail "$block_status-BT_RF_DUT"

		eval $hci0_command
		exit_on_fail "bring_$hci_status-BT_DUT"

	else
		ssh $SSH_PARAMS root@$hostIP "$rfkill_command"
		exit_on_fail "$block_status-BT_RF_HELPER"

		ssh $SSH_PARAMS root@$hostIP "$hci0_command"
		exit_on_fail "bring_$hci_status-BT_HELPER"
	fi
}

###############################################################################
### makes that other devices can find HELPER/DUT device in BT scan
### arg1 = expects string "HELPER" or "DUT"
###############################################################################
make_discoverable()
{
	local DEVICE_TO_MAKE_DISCOVERABLE=$1

	if [ $DEVICE_TO_MAKE_DISCOVERABLE = "DUT" ]; then
		info_msg "===========making DUT discoverable====================="
		hciconfig hci0 piscan
	elif [ $DEVICE_TO_MAKE_DISCOVERABLE = "HELPER" ]; then
		info_msg "===========making HELPER discoverable====================="
		ssh $SSH_PARAMS root@$HELPER_IP_ADDR "hciconfig hci0 piscan"
	else
		error_msg "wrong parameter given to function make_discoverable"
	fi
}

###############################################################################
### function runs when scripts exist. turns off BT in DUT and in HELPER
### usage: trap close_BT EXIT
###############################################################################
close_BT()
{
	info_msg "--EXIT-script-closing-BT-in-HELPER-and-DUT--"
	change_BT_status "down"
	change_BT_status "down" $HELPER_IP_ADDR
}

###############################################################################
##### enables pulseaudio on DUT
###############################################################################
enable_pulse_audio()
{
	export DISPLAY=:0.0
	check_return "export DISPLAY"

	killall pulseaudio

	pulseaudio --exit-idle-time=1000000 -D
	check_return "start-pulseaudio"
}


#########################################################
### function returns 0 if there is transmission from DUT to HELPER over BT
### and that HELPER receives BT transmissions
### otherwise returns 1
### arg1 = duration in seconds for checking for BT transmission
#########################################################
is_DUT_BT_transmitting_to_HELPER()
{
	local time_seconds=$1
	local TX_total_bytes=0
	local RX_total_bytes=0

	#DUT TX
	local TX_CURR="$(hciconfig -a | grep "TX" | cut -d":" -f2 | cut -d" " -f1)"
	local TX_delta=$TX

	#HELPER RX
	local RX_CURR="$(ssh $SSH_PARAMS root@$HELPER_IP_ADDR "hciconfig -a | grep "RX" | cut -d':' -f2 | cut -d' ' -f1")"
	local RX_delta=$RX

	for I in $(seq $time_seconds); do
		#DUT TX iteration
		TX_CURR="$(hciconfig -a | grep "TX" | cut -d":" -f2 | cut -d" " -f1)"
		TX_delta=$((TX_CURR - TX_delta))
		TX_total_bytes=$((TX_total_bytes + TX_delta))			

		#HELPER RX iteration
		RX_CURR="$(ssh $SSH_PARAMS root@$HELPER_IP_ADDR "hciconfig -a | grep "RX" | cut -d':' -f2 | cut -d' ' -f1")"
		RX_delta=$((RX_CURR - RX_delta))
		RX_total__bytes=$((RX_total_bytes + RX_delta))			

		time_seconds=$((time_seconds - 1))
		sleep 1
	done

	if [ $TX_total_bytes -gt 0 -a $RX_total__bytes -gt 0 ]; then
		return 0
	else 
		return 1
	fi
}


###############################################################################
### functions receives a path to a script to run. runs it and checks it's return status
### arg1 = path of script to executed
###############################################################################
run_and_check()
{
	local script_path=$1

	$script_path
	exit_on_fail "$script_path"
}


