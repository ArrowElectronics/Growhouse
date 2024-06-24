#!/usr/bin/expect -f
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

############################################################################
### description: pairing from DUT to HELPER
### receives 3 arguments
### arg1 - HELPER MAC address - script will search this address in BT scan logs
### arg2 - expect script debug configuration
### arg3 - expect script output configuration
############################################################################

# using "source" because expect do not support "."
source ../../utils/BT-tests-util/BT-expect-lib.sh

# parse arguments
set HELPER_MAC_ADDR [lindex $argv 0]
set DEBUG_OPTION [lindex $argv 1]
set OUTPUT_OPTION [lindex $argv 2]

####################### script execution #############################

# Debug Option
exp_internal $DEBUG_OPTION

# Output Option
log_user $OUTPUT_OPTION

set PROMPT "#"

spawn bluetoothctl
sleep 2

#Commands in bluetoothctl prompt to scan and pair to helper device
run_command "#" "power on" "Changing power on succeeded"
run_command "#" "remove *"
run_command "#" "agent NoInputNoOutput" "Agent registered" "Agent is already registered"
run_command "#" "default-agent" "Default agent request successful"
run_command "#" "scan on" "Discovery started" "Failed to start discovery: org.bluez.Error.InProgress"

#timeout for next expect statements
set timeout 50
# wait for device to be found
expect {
	timeout { send_user "\nbluetoothctl timed out timeout=$timeout\n"; exit 1 }
	eof { send_user "\nbluetoothctl was closed (expect got EOF)\n"; exit 1 }
	"Device $HELPER_MAC_ADDR" { send_user "\n===device=$HELPER_MAC_ADDR found in scan!====\n" }
}

run_command "#" "pair $HELPER_MAC_ADDR" "Pairing successful" "Failed to pair: org.bluez.Error.AlreadyExists"

sleep 2

expect -re $PROMPT
send "quit\r"
expect eof
