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
. ../../lib/math/decimal_numbers.sh
. ../../lib/ping-router.sh

default_max_response_time=5 #ms
default_ping_time=2 #s

ROUTER_IP=""
CONNECTION_NAME=""

OTHER_CONNECTIONS=""

MAX_RESPONSE_TIME=$1
PING_TIME=$2



get_router_ip() {
  ethernet_router=$(ip route show | grep "default via" | grep "dev e")
  if [ "$ethernet_router" = "" ]; then
    report_fail "No_router_found"
    return 1
  fi
  
  ROUTER_IP=$(echo $ethernet_router | awk '{print $3 }')
  CONNECTION_NAME=$(echo $ethernet_router | awk '{print $5 }')
  
  if [ "$ROUTER_IP" = "" ]; then
    report_fail "Unable_to_parse_router_ip_output"
    return 1
  fi
  
  OTHER_CONNECTIONS=$(ip route show | grep "default via" | grep -v "$CONNECTION_NAME" | awk '{print $5 }')
  return 0
}



if [ -z "$MAX_RESPONSE_TIME" ]; then
  info_msg "Maximum response time is not given, defaulting to $default_max_response_time"
  MAX_RESPONSE_TIME=$default_max_response_time
fi

if [ -z "$PING_TIME" ]; then
  info_msg "Check time is not given, defaulting to $default_ping_time"
  PING_TIME=$default_ping_time
elif [ $PING_TIME -lt $default_ping_time ]; then
  info_msg "Check time given is less then the minimun, $default_ping_time will be used instead"
  PING_TIME=$default_ping_time
fi

get_router_ip
exit_on_fail "Get_IP_of_router"

disable_interfaces "$OTHER_ROUTERS"
ping_router "$PING_TIME" "$ROUTER_IP"
cat $PING_LOG
enable_interfaces "$OTHER_ROUTERS"

analyze_ping_output "$ROUTER_IP" "$MAX_RESPONSE_TIME"
exit_on_fail "Analyze_ping_output"

report_pass "Ethernet_works_with_satisfying_speed"
