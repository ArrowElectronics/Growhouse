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

default_button_press_timeout=3

UP_OR_DOWN="$1"
LOG_FILE="$2"
BUTTON_PRESSURE_TIMEOUT="$3"
INITIAL_VOLUME="50"

analyze_input_parameters() {
  if_up=$(echo "$UP_OR_DOWN" | grep -ix "up")
  if_down=$(echo "$UP_OR_DOWN" | grep -ix "down")
  if [ "if_up" = "" -a "if_down" = "" ]; then
    echo "Input should be \"UP\" of \"DOWN\""
    return 1
  fi
  
  touch "$LOG_FILE"
  if [ ! -f "$LOG_FILE" ]; then
    echo "Log file $LOG_FILE doesn't exist and can't be created"
    return 1
  fi
  
  if [ -z "$BUTTON_PRESSURE_TIMEOUT" ]; then
    echo "Timeout of button pressing is not given. Defaulting to $default_button_press_timeout"
    BUTTON_PRESSURE_TIMEOUT="$default_button_press_timeout"
    
  # check that given time out is a positive integer
  elif [ ! -z `echo $BUTTON_PRESSURE_TIMEOUT | tr -d "[0-9]"` ]; then
    echo "$BUTTON_PRESSURE_TIMEOUT is invalid timeout"
    return 1
  fi
  
  return 0
}

analyze_volume_change() {
  old_result=$INITIAL_VOLUME
  new_result_left=$(su - linaro -c "amixer get Master" | grep "Front Left: Playback" | awk -F"[][]" '{print $2}' | awk -F"%" '{print $1}')
  new_result_right=$(su - linaro -c "amixer get Master" | grep "Front Right: Playback" | awk -F"[][]" '{print $2}' | awk -F"%" '{print $1}')
  
  local comparison_sign=""
  
  if [ ! "$if_up" = "" ]; then
    comparison_sign=">"
  elif [ ! "$if_down" = "" ]; then
    comparison_sign="<"
  else
    echo "Something went wrong. It should never happend"
    return 1
  fi
  
  if [ $new_result_left $comparison_sign $old_result -o $new_result_right $comparison_sign $old_result ]; then
    return 0
  else
    return 1
  fi
}

timer() {
  local j="$1"
  local fr=0.1
  local r=10
  local r_curr="$r"
  local unset curr_volume_l
  local unset curr_volume_r
  printf ' '
  sleep 0.1
  
  while [ $j -gt 0 ]; do
    printf '\rYou have %s s to do that   ' "${j}"
    j=$(($j - 1))
    
    #sleep 1
    while [ $r_curr -gt 0 ]; do
      analyze_volume_change
      if [ "$?" -eq 1 ]; then
        r_curr=$(($r_curr - 1))
        sleep $fr
        continue
      else
        printf '\r                          \n'
        return 0
      fi
    done
    r_curr="$r"
  done

  printf '\r                          \n'
  return 1
}

reports_results() {
  old_result=$INITIAL_VOLUME
  new_result_left=$(su - linaro -c "amixer get Master" | grep "Front Left: Playback" | awk -F"[][]" '{print $2}' | awk -F"%" '{print $1}')
  new_result_right=$(su - linaro -c "amixer get Master" | grep "Front Right: Playback" | awk -F"[][]" '{print $2}' | awk -F"%" '{print $1}')
  
  echo "Previous volume: ${old_result}% with new volume for left: ${new_result_left}% and for right: ${new_result_right}%"
}

check_results() {
  local res="$?"
  local message="$1"
  
  if [ "$res" -ne 0 ]; then
    echo "$message fail" >> "$LOG_FILE"
  else
    echo "$message pass" >> "$LOG_FILE"
  fi
}

analyze_input_parameters
check_results "${UP_OR_DOWN} is ligal parameter. It shoild be UP or DOWN"

su - linaro -c "amixer --quiet set Master \"$INITIAL_VOLUME\"%"

echo ""
echo "Please press the \"Volume $UP_OR_DOWN\" button on the DUT ones"

timer $BUTTON_PRESSURE_TIMEOUT
check_results "Volume change"
reports_results
