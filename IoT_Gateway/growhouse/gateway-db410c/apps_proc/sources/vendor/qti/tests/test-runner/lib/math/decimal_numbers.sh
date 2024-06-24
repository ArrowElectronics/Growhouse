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

# cheks if string is positive integer or decimal
if_number() {
  local number="$1"
  
  if [ "$number" = "" ]; then
    return 1
  fi
  
  local bdec=$(echo "$number" | awk -F'.' '{print $1}' )
  local adec=$(echo "$number" | awk -F'.' '{print $2}' )
  
  if [ "$bdec" = "" ]; then
    return 1
  fi
  
  if [ $bdec -eq $bdec ] 2>/dev/null; then
    if [ "$adec" = "" -o $adec -eq $adec ] 2>/dev/null; then
      return 0
    fi
  fi
  
  return 1
}

# check if the first given positive decimal or integer is bigger then the second one
if_bigger() {
  local bigger="$1"
  local smaller="$2"
  
  local bdec_bigger=$(echo "$bigger" | awk -F'.' '{print $1}' )
  local adec_bigger=$(echo "$bigger" | awk -F'.' '{print $2}' )
  if [ "$adec_bigger" = "" ]; then
    adec_bigger=0
  fi
  
  local bdec_smaller=$(echo "$smaller" | awk -F'.' '{print $1}' )
  local adec_smaller=$(echo "$smaller" | awk -F'.' '{print $2}' )
  if [ "$adec_smaller" = "" ]; then
    adec_smaller=0
  fi
  
  if [ $bdec_bigger -gt $bdec_smaller ]; then
    return 0
  elif [ $bdec_bigger -lt $bdec_smaller ]; then
    return 1
  else
    local bigger_len=${#adec_bigger}
    local smaller_len=${#adec_smaller}
    
    if [ $bigger_len -gt $smaller_len ]; then
      local dif_len=$(( $bigger_len - $smaller_len )) 
      adec_smaller=$(( $adec_smaller * $dif_len * 10 ))
    elif [ $bigger_len -lt $smaller_len ]; then
      local dif_len=$(( $smaller_len - $bigger_len )) 
      adec_bigger=$(( $adec_bigger * $dif_len * 10 ))
    fi
    
    if [ $adec_bigger -gt $adec_smaller ]; then
      return 0
    else
      return 1
    fi
  fi
}

# check if the first given positive decimal or integer is smaller then the second one
if_smaller() {
  local smaller="$1"
  local bigger="$2"
  
  local bdec_smaller=$(echo "$smaller" | awk -F'.' '{print $1}' )
  local adec_smaller=$(echo "$smaller" | awk -F'.' '{print $2}' )
  if [ "$adec_smaller" = "" ]; then
    adec_smaller=0
  fi
  
  local bdec_bigger=$(echo "$bigger" | awk -F'.' '{print $1}' )
  local adec_bigger=$(echo "$bigger" | awk -F'.' '{print $2}' )
  if [ "$adec_bigger" = "" ]; then
    adec_bigger=0
  fi
  
  if [ $bdec_smaller -lt $bdec_bigger ]; then
    return 0
  elif [ $bdec_smaller -gt $bdec_bigger ]; then
    return 1
  else
    local smaller_len=${#adec_smaller}
    local bigger_len=${#adec_bigger}
    
    if [ $bigger_len -gt $smaller_len ]; then
      local dif_len=$(( $bigger_len - $smaller_len )) 
      adec_smaller=$(( $adec_smaller * $dif_len * 10 ))
    elif [ $bigger_len -lt $smaller_len ]; then
      local dif_len=$(( $smaller_len - $bigger_len )) 
      adec_bigger=$(( $adec_bigger * $dif_len * 10 ))
    fi
    
    if [ $adec_smaller -lt $adec_bigger ]; then
      return 0
    else
      return 1
    fi
  fi
}

# check if the first given positive decimal or integer is bigger or equal then the second one
if_bigger_or_equal() {
  if_smaller "$1" "$2"
  local res="$?"
  
  if [ $res -eq 1 ]; then
    return 0
  else
    return 1
  fi
}

# check if the first given positive decimal or integer is smaller or equal then the second one
if_smaller_or_equal() {
  if_bigger "$1" "$2"
  local res="$?"
  
  if [ $res -eq 1 ]; then
    return 0
  else
    return 1
  fi
}

