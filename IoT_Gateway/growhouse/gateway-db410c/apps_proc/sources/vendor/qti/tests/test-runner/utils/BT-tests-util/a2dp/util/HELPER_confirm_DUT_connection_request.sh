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

############################### description ###################################
### activating BT on HELPER and confirming DUT's request for connection
####################### script execution ######################################

# using "source" because expect do not support "."
# access to BT expect functions and general test configurations (e.g HELPER_USER_NAME)
set CURR_DIR_PATH [file dirname [info script]]
source $CURR_DIR_PATH/../../BT-expect-lib.sh

# Debug Option
exp_internal $DEBUG_OPTION

# Output Option
log_user $OUTPUT_OPTION


set remote_host "$HELPER_USER_NAME@"
append remote_host $HELPER_IP_ADDR

set timeout 20
spawn ssh $SSH_PARAMS $remote_host "bluetoothctl"
expect { 
	eof { send_user "\nHELPER bluetoothctl was closed (expect got EOF)\n"; exit 1 }
	timeout { send_user "\nHELPER bluetoothctl timed out timeout=$timeout\n"; exit 1 }
	"password" { send "$HELPER_USER_PASSWORD\r" } 
}
set timeout 60
sleep 2
run_command "#" "power on" "Changing power on succeeded"
#TODO test this - maybe you wont need to replay to a now yes question
#seting agent as NoInputNoOutput makes it pair without need to user confirmation at HELPER
run_command "#" "agent NoInputNoOutput" "Agent registered" "Agent is already registered"
run_command "#" "default-agent" "Default agent request successful"
run_command "#" "discoverable on" "Changing discoverable on succeeded"

set timeout 120
expect {
	eof { send_user "\nHELPER bluetoothctl (expect got EOF) waiting for DUT\
to connect\n"; exit 1 }
	timeout { send_user "\nHELPER bluetoothctl timed out timeout=$timeout\
waiting for DUT to connect \n"; exit 1 }
	"yes/no" { send "yes\r" }
} 

set PROMPT "#"

sleep 2

expect -re $PROMPT
send "quit\r"
expect eof

