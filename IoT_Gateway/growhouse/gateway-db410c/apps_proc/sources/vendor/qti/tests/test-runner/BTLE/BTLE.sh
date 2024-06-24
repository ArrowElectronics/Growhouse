#!/bin/sh
#Copyright (c) 2018, The Linux Foundation. All rights reserved.
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

# set result logging functions
. ../../lib/sh-test-lib
. ../../lib/set-result-dir.sh

min_scan_seconds=2
default_scan_seconds=7

SCAN_LOG="scan.log"
SCAN_ATTEMPTS=5

SCAN_SECONDS="$1"
BTLE_MAC="$2"
IF_DEV_DEFINED=false

analyze_input_parameters() {
  if [ -z "$BTLE_MAC" ]; then
    info_msg "MAC of BTLE device is not given. Scaning for any device"
    IF_DEV_DEFINED=false
  else
    IF_DEV_DEFINED=true
  fi

  if [ -z "$SCAN_SECONDS" ]; then
    info_msg "Time to scan for BTLE device in not given. Defaulting to $default_scan_seconds"
    SCAN_SECONDS=$default_scan_seconds
  elif [ $SCAN_SECONDS -eq $SCAN_SECONDS ]; then
    if [ $SCAN_SECONDS -lt $min_scan_seconds ]; then
      info_msg "time to scan for BTLE device can't be less then $min_scan_seconds"
      SCAN_SECONDS=$min_scan_seconds
    fi
  else
    info_msg "Time to scan given: $SCAN_SECONDS is not a valid number"
    return 1
  fi

  return 0
}

# this finction is currently required because on Debian (wich is the test environment) build bluetooth is not activated automaticly
activate_bluetooth() {
  btattach -B /dev/ttyMSM1 -P qca &
  sleep 2
}

scan_for_BTLE_devices() {
  local pid_of_scan=""
  local error_message=""
  local a=0

  while [ $a -lt $SCAN_ATTEMPTS ]
  do
    timeout -s INT ${SCAN_SECONDS}s hcitool lescan > $SCAN_LOG 2>&1
    cat $SCAN_LOG
    error_message=$(cat $SCAN_LOG | grep -i "fail")
    if [ ! "$error_message" = "" ]; then
      hciconfig dev reset
      sleep 2
      continue
    else
      break
    fi
  done
}

analyze_scan_output() {
  local output=""

  if [ $IF_DEV_DEFINED = true ]; then
    output=$(cat "$SCAN_LOG" | grep -i "$BTLE_MAC")
  else
    output=$(cat "$SCAN_LOG" | grep -m 1 -o -E '([[:xdigit:]]{1,2}:){5}[[:xdigit:]]{1,2}')
  fi

  rm "$SCAN_LOG"
  if [ "$output" = "" ]; then
    return 1
  else
    return 0
  fi
}

cleanup() {
  bt_pid=$(pidof btattach)
  kill -9 $bt_pid
}

analyze_input_parameters
exit_on_fail "Analyze_input_parameters"

activate_bluetooth
scan_for_BTLE_devices

analyze_scan_output
check_return "Scan_for_BTLE_device"

cleanup
