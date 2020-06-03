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

# !!!
# REQUIRES INCLUDE OF decimal_numbers.sh LIBRARY BEFORE INCLUDE OF THAT LIBRARY
# !!!

# parameters:
# ping time - number of seconds to perform ping
# router ip - ip address to ping
ping_router() {
  PING_TIME="$1"
  ROUTER_IP="$2"
  
  PING_LOG="ping${ROUTER_IP}.log"
  echo "ping log - $PING_LOG"
  ping $ROUTER_IP > $PING_LOG 2>&1 &
  
  ping_pid=$(pidof ping)
  sleep $PING_TIME
  kill -9 $ping_pid
}

# parameters:
# router ip - ip address to ping
# max response time - maximum time a response should take
analyze_ping_output() {
  ROUTER_IP="$1"
  MAX_RESPONSE_TIME="$2"

  PING_LOG="ping${ROUTER_IP}.log"
  echo "ping log - $PING_LOG"
  
  time_output=$(cat $PING_LOG | awk -F" |=" '{print $10}')

  local valid_output_found=false
  local time_out_of_range=false
  
  for i in $time_output
  do
    # check if i is a valid number
    if_number "$i"
    if [ "$?" = "0" ]; then
      valid_output_found=true
    else
      continue
    fi
    
    # check if i isn't bigger then the maximum time
    if_bigger "$i" "$MAX_RESPONSE_TIME"
    if [ $? -eq 0 ]; then
      time_out_of_range=true
      info_msg "$i response time is bigger then maximun:$MAX_RESPONSE_TIME"
    fi
  done
  
  if [ "$valid_output_found" = "false" ]; then
    report_fail "No_valid_output_found"
    return 1
  elif [ "$time_out_of_range" = "true" ]; then
    report_fail "Response_time_is_too_big"
    return 1
  fi
}

# parameters
# list of interfaces to disable
disable_interfaces() {
  INTERFACES_LIST="$1"

  for i in $INTERFACES_LIST
  do
    info_msg "disactivating $i"
    ifconfig $i down
  done
}

# parameters
# list of interfaces to disable
enable_interfaces() {
  INTERFACES_LIST="$1"

  for i in $INTERFACES_LIST
  do
    info_msg "activating $i"
    ifconfig $i up
  done
}

