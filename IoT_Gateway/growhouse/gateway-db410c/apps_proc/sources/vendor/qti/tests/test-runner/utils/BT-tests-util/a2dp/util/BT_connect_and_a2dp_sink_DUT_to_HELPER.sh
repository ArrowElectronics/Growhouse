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
### connecting DUT to HELPER 
### setting a2dp as BT sink on DUT
### after this script DUT is ready to play audio to be heard on HELPER USB speakers

# using "source" because expect do not support "."
# access to BT expect functions and general test configurations (e.g HELPER_USER_NAME)
set CURR_DIR_PATH [file dirname [info script]]
source $CURR_DIR_PATH/../../BT-expect-lib.sh

####################### script execution #############################

# Debug Option
exp_internal $DEBUG_OPTION

# Output Option
log_user $OUTPUT_OPTION

# parse arguments
set HELPER_MAC_ADDR [lindex $argv 0]


set PROMPT "#"

spawn bluetoothctl
sleep 2
run_command "#" "power on" "Changing power on succeeded"
#removes device in case it was paired before test
run_command "#" "remove *"
run_command "#" "agent NoInputNoOutput" "Agent registered" "Agent is already registered"
#run_command "default-agent" "Default agent request successful"
sleep 5
run_command "#" "scan on" "Discovery started" "Failed to start discovery: org.bluez.Error.InProgress"
#timeout for next expect statements
set timeout 100
# wait for device to be found
expect {
      timeout { send_user "\nbluetoothctl timed out timeout=$timeout\n"; exit 1 }
      eof { send_user "\nbluetoothctl was closed (expect got EOF)\n"; exit 1 }
      "Device $HELPER_MAC_ADDR" { send_user "\n===device=$HELPER_MAC_ADDR found in scan!====\n" }
}
sleep 5
run_command "#" "pair $HELPER_MAC_ADDR" "Pairing successful" "Failed to pair: org.bluez.Error.AlreadyExists"
run_command "#" "trust $HELPER_MAC_ADDR" "Changing $HELPER_MAC_ADDR trust succeeded"
sleep 15
run_command "#" "connect $HELPER_MAC_ADDR" "Connection successful"

sleep 10

expect -re $PROMPT
send "quit\r"
expect eof

# Sets the a2dp_sink profile to the bluez_card
#TODO do we need this printing out?
spawn pactl list sinks short
expect eof

# altering MAC address to create bulez card name
set DEVICE_MAC_ADDR_ALT [regsub -all ":" $HELPER_MAC_ADDR "_"]

set DEVICE_CARD_NAME "bluez_card."
append DEVICE_CARD_NAME $DEVICE_MAC_ADDR_ALT

spawn pactl set-card-profile $DEVICE_CARD_NAME a2dp_sink
expect {
          timeout { send_user "\nset card profile timed out timeout=$timeout\n"; exit 1 }
          "Failure: No such entity" { send_user "\ndoesn't find specified\
card=$DEVICE_CARD_NAME\n"; exit 1 }
          eof { send_user "\n===$DEVICE_CARD_NAME setted as card profile====\n" }
        }

spawn pactl list sinks short
expect {
          timeout { send_user "\nlist sinks timed out timeout=$timeout\n"; exit 1 }
          eof { send_user "\ndoesn't find MAC\n"; exit 1 }
          "bluez_sink.$DEVICE_MAC_ADDR_ALT" { send_user "\n===bluez_sink\
.$DEVICE_MAC_ADDR_ALT found====\n" }
}


# Sets the bluetooth speaker as the default sink
set DEVICE_SINK_NAME "bluez_sink."
append DEVICE_SINK_NAME $DEVICE_MAC_ADDR_ALT

spawn pactl set-default-sink $DEVICE_SINK_NAME
expect {
          timeout { send_user "\nset default sink timed out timeout=$timeout\n"; exit 1 }
          "Failure: No such entity" { send_user "\ndoesn't find specified sink\n"; exit 1 }
          eof { send_user "\n===$DEVICE_SINK_NAME setted as default sink====\n" }
        }

sleep 10

## Run an audio sample
#spawn aplay /usr/share/sounds/alsa/Front_Right.wav
#expect {
#	timeout { send_user "Timed out to run aplay command!\n" ; exit 1 }
#	eof { send_user "aplay command performed with success!\n" }
#}

