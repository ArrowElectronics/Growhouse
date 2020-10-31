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
# requires ../sh-test-lib added before this lib

hexa_prefix_lc="0x"
hexa_prefix_uc="0X"

# checks if string is hexadecimal

if_hexa() {
  local string_to_check="$1"
  local hex_part_of_string=""
  
  if [ -z "${string_to_check##$hexa_prefix_lc*}" ]; then
    hex_part_of_string=${string_to_check#$hexa_prefix_lc}
  elif [ -z "${string_to_check##$hexa_prefix_uc*}" ]; then
    hex_part_of_string=${string_to_check#$hexa_prefix_uc}
  else
    info_msg "$string_to_check is not hexadecimal"
    return 1
  fi
  
  if [ "$hex_part_of_string" = "" ]; then
    info_msg "$string_to_check is not hexadecimal"
    return 1
  fi
  
  if [ -z `echo $hex_part_of_string | tr -d "[a-fA-F0-9]"` ]; then
    #info_msg "$string_to_check is hexadecimal"
    return 0
  else
    info_msg "$string_to_check is not hexadecimal"
    return 1
  fi
}

# checks if the first given hexadecimal number is greater then the second one

if_hexa_is_greater() {
  local first_hexa="$1"
  local second_hexa="$2"
  local first_hexa_part=""
  local second_hexa_part=""
  local first_dec_num=0
  local second_dec_num=0
  
  if [ -z "$first_hexa" ]; then
    info_msg "Input to check is not given"
    return 1
  fi
  
  if [ -z "$second_hexa" ]; then
    info_msg "Number to compare to is not given"
    return 1
  fi
  
  first_hexa=$(echo $first_hexa | tr [a-z] [A-Z])
  second_hexa=$(echo $second_hexa | tr [a-z] [A-Z])
  
  first_hexa_part=${first_hexa#$hexa_prefix_uc}
  if [ "$first_hexa_part" = "" ]; then
    info_msg "$first_hexa_part is not hexadecimal"
    return 1
  fi
  second_hexa_part=${second_hexa#$hexa_prefix_uc}
  if [ "$second_hexa_part" = "" ]; then
    info_msg "$second_hexa_part is not hexadecimal"
    return 1
  fi
  
  first_dec_num=$(echo "ibase=16; $first_hexa_part"|bc)
  second_dec_num=$(echo "ibase=16; $second_hexa_part"|bc)
  
  if [ $first_dec_num -gt $second_dec_num ]; then
    return 0
  else
    return 1
  fi
}

# checks if the first given hexadecimal number is less then the second one

if_hexa_is_less() {
  local first_hexa="$1"
  local second_hexa="$2"
  local first_hexa_part=""
  local second_hexa_part=""
  local first_dec_num=0
  local second_dec_num=0
  
  if [ -z "$first_hexa" ]; then
    info_msg "Input to check is not given"
    return 1
  fi
  
  if [ -z "$second_hexa" ]; then
    info_msg "Number to compare to is not given"
    return 1
  fi
  
  first_hexa=$(echo $first_hexa | tr [a-z] [A-Z])
  second_hexa=$(echo $second_hexa | tr [a-z] [A-Z])
  
  first_hexa_part=${first_hexa#$hexa_prefix_uc}
  if [ "$first_hexa_part" = "" ]; then
    info_msg "$first_hexa_part is not hexadecimal"
    return 1
  fi
  second_hexa_part=${second_hexa#$hexa_prefix_uc}
  if [ "$second_hexa_part" = "" ]; then
    info_msg "$second_hexa_part is not hexadecimal"
    return 1
  fi
  
  first_dec_num=$(echo "ibase=16; $first_hexa_part"|bc)
  second_dec_num=$(echo "ibase=16; $second_hexa_part"|bc)
  
  if [ $first_dec_num -lt $second_dec_num ]; then
    return 0
  else
    return 1
  fi
}
