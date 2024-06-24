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
### setting the usb speaker/headphones as default audio in HELPER device.
### connecting to HELPER with user privileges
### run pulseaudio and set USB speaker/headphones as default sink

####################### script execution ######################################

# using "source" because expect do not support "."
# access to BT expect functions and general test configurations (e.g HELPER_USER_NAME)
set CURR_DIR_PATH [file dirname [info script]]
source $CURR_DIR_PATH/../../BT-expect-lib.sh

# Debug Option
#exp_internal $DEBUG_OPTION
exp_internal 1
# Output Option
#log_user $OUTPUT_OPTION
log_user 1

set HELPER_ADDR_ROOT "root@$HELPER_IP_ADDR"

set SSH_PARAM1 -Y

set remote_host "$HELPER_USER_NAME@"
append remote_host $HELPER_IP_ADDR

append SSH_PARAMS " -Y "

spawn ssh $SSH_PARAMS $HELPER_ADDR_ROOT "killall pulseaudio"
expect eof

exit 1

set HELPER_ADDR_USER $HELPER_USER_NAME@$HELPER_IP_ADDR
#append HELPER_ADDR_USER $HELPER_IP_ADDR

spawn ssh $SSH_PARAMS $HELPER_ADDR_USER "export DISPLAY=:0.0; pulseaudio --exit-idle-time=1000000 -D"
expect {
	eof { }		 
	"password: " { send "$HELPER_USER_PASSWORD\r" ; expect eof}
}


###########TODO###############
#exit 1


spawn ssh $SSH_PARAMS $HELPER_ADDR_USER "pactl list sinks short | grep usb | awk '{print \$2}'"
expect {
	eof { } 
	"password: " { send "$HELPER_USER_PASSWORD\r" ; expect eof}
}

#set DEVICE_SINK_NAME [lindex [split $expect_out(buffer) \n] 2 ]
set DEVICE_SINK_NAME $expect_out(buffer)

send_user "=====DEVICE_SINK_NAME=$DEVICE_SINK_NAME====="

if { $DEVICE_SINK_NAME eq "" } {
	send_user "USB device not found on the sinks list!\n"
	exit 1
}

#send_user "############### DEVICE_SINK_NAME=$DEVICE_SINK_NAME ###############\n"

set DEVICE_SINK_NAME [ string range $DEVICE_SINK_NAME 0 end-1 ]

spawn ssh $SSH_PARAMS $HELPER_ADDR_USER "pactl set-default-sink $DEVICE_SINK_NAME"
expect {
		 
	"password: " { 
		send "$HELPER_USER_PASSWORD\r"  
		expect {
			timeout { send_user "\nset default sink timed out timeout=$timeout\n"; exit 1 }
    		"Failure: No such entity" { send_user "\ndoesn't find specified sink\n"; exit 1 }
		}
	}
	eof { }
	"Failure: No such entity" { send_user "\ndoesn't find specified sink\n"; exit 1 }
}


spawn ssh $SSH_PARAMS $HELPER_ADDR_USER "pactl info"
expect {
	"password: " {
		send "$HELPER_USER_PASSWORD\r"
		expect {
			timeout { send_user "Timed out to find $DEVICE_SINK_NAME as default sink\n" }
			eof { }
			"$DEVICE_SINK_NAME" { send_user "$DEVICE_SINK_NAME setted as default sink\n" }
		}
	}
	"$DEVICE_SINK_NAME" { send_user "$DEVICE_SINK_NAME setted as default sink\n" }
	eof { }
}


