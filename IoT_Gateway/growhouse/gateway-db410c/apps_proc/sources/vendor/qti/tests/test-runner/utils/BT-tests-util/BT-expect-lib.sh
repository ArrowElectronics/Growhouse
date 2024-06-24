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

#################################### BT-lib ####################################################

#############################
# function executes a command in bluetoothctl and expect first or second(if present) return values
# arg1 - command
# arg2 - expected return value for a successful command
# arg3 - second expected return value, if first not found.
#############################
#TODO change this scripts name
proc run_command args {

  # parse args
    set PROMPT [lindex $args 0]
    set _command [lindex $args 1]
    set _expected_ret_val1 [lindex $args 2]
    set _expected_ret_val2 [lindex $args 3]

    send_user "\n =========================== starting command=\"$_command\" in function run_command ====================== \n"

    send_user "\n ==== arg1 command=\"$_command\" arg2=\"$_expected_ret_val1\" arg3=\"$_expected_ret_val2\"=== \n"

    set timeout 20

    expect {
        timeout { send_user "\nbluetoothctl timed out timeout=$timeout\n"; exit 1 }
        eof { send_user "\nbluetoothctl was closed (expect got EOF)\n"; exit 1 }
        "$PROMPT" { send "$_command\r" }
    }

    # if arg1 found set flag so it would not continue to look for arg2
    # 0=found, 1=not found
    set first_expected_ret_val_found_flag 0
    expect {
        timeout { send_user "\n====timed ot to get expected return val1=$_expected_ret_val1====\n"; set first_expected_ret_val_found_flag 1}
        eof { send_user "\n=======bluetoothctl was closed (expect got EOF)======\n"; exit 1 }
        "$_expected_ret_val1" { }
    }

    if { $first_expected_ret_val_found_flag == 0 } {
        send_user "\n ============== first expected ret val=$_expected_ret_val1 found!======== \n"
        send_user "\n =========== flag=$first_expected_ret_val_found_flag (0=found, 1=not found) ============ \n"
        send_user "\n =========== not cheking second ============= \n"
        return 0        
    } else {
        send_user "\n ============== first expected ret val=$_expected_ret_val1 NOT found! ========= \n"
        send_user "\n ============== flag=$first_expected_ret_val_found_flag (0=found, 1=not found) ======= \n"
        send_user "\n ============== cheking second=$_expected_ret_val2 ============= \n"
        if { $_expected_ret_val2 eq ""} {
            send_user "\n ============== second parameter is empty ============= \n"
            exit 1
        }
    }

    send_user "\n====start expecting expected return val2=$_expected_ret_val2====\n"

    expect {
        timeout { send_user "\n====timed out to get expected return val2=$_expected_ret_val2====\n"; exit 1 }
        eof { send_user "\n=======bluetoothctl was closed (expect got EOF)======\n"; exit 1 }
        "$_expected_ret_val2" { }
    }


    send_user "\n =========================== finished command=$_command function run_command ====================== \n"
}

proc parse_config_val args {

    set parse_value [lindex $args 0]
	
	#if this script is sourced from other pathes, it will always fined BT-tests-config.sh
    set path [file dirname [info script]]
    set BT_CONFIG $path/BT-tests-config.sh

    spawn sh -c "grep -v '#' $BT_CONFIG | grep $parse_value"
    expect eof

	set quoted_retval $expect_out(buffer)

	set not_quoted_retval [regsub -all {"} $quoted_retval ""]
	set trimed_not_quoted_retval [ string trim $not_quoted_retval ]

	set delimIndex [string first "=" $trimed_not_quoted_retval]
	set strlen [ string length $trimed_not_quoted_retval ]
	set parsedValue [string range $trimed_not_quoted_retval $delimIndex+1 $strlen-1]

	return $parsedValue
}

###############################################################################
### parsing configuration values form BT-tests-config.sh file for expect scripts
### this way the config values will be set in only one file
###############################################################################
set HELPER_IP_ADDR [ parse_config_val HELPER_IP_ADDR ]

set DEBUG_OPTION [ parse_config_val DEBUG_OPTION ]

set OUTPUT_OPTION [ parse_config_val OUTPUT_OPTION ]

set HELPER_USER_NAME [ parse_config_val HELPER_USER_NAME ]

set HELPER_USER_PASSWORD [ parse_config_val HELPER_USER_PASSWORD ]

set SSH_PARAMS [ parse_config_val SSH_PARAMS ]
