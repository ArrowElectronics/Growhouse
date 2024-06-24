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
#          ../set-result-dir.sh
#          ../math/hexadecimals.sh
#          ../math/integers.sh

min_chip_address="0x03"
max_chip_address="0x77"

max_data_address="0xff"
max_value_to_write_hexa="0xff"
max_value_to_write_int=255


########	I2C HELP FUNCTIONS

# gives list of i2c buses numbers available 

i2c_get_buses() {
  i2c_buses_list=$(i2cdetect -l | awk -F"[-'\t']" -v ORS=" " '{print $2}')
  info_msg "Available buses: $i2c_buses_list"
}

# checks if i2c bus number given is in the list of available i2c buses

check_if_i2c_bus_exists() {
  local bus_to_check="$1"

  if [ -z "$bus_to_check" ]; then
    info_msg "bus to check is not defined"
    return 1
  fi

  i2c_get_buses
  for i in $i2c_buses_list
  do
    if [ $i -eq $bus_to_check ]; then
      return 0
    fi
  done
  
  return 1
}

# gives list if chip addresses available for given i2c bus, if such bus is defined
# if i2c bus isn't defined, returns list of chip addresses for all i2c buses available

i2c_get_chip_addresses() {
  local i2c_bus="$1"
  local chip_addresses_raw=""
  local chip_addresses_final=""
  local unset bus
  local unset ca
  
  if [ -z "$i2c_bus" ]; then
    i2c_get_buses
    i2c_bus="$i2c_buses_list"
  else
    check_if_i2c_bus_exists $i2c_bus
    if [ "$?" -ne 0 ]; then
      info_msg "i2c bus $i2c_bus doesn't exist"
      return 1
    fi
  fi
  
  for bus in $i2c_bus
  do
    chip_addresses_final=""
    chip_addresses_raw=$(i2cdetect -y -r $bus | grep : | awk -F":" -v ORS=" " '{print $2}' | tr - " ")
    for ca in $chip_addresses_raw
    do
      chip_addresses_final="$chip_addresses_final $ca"
    done
    info_msg "Available chip addresses for i2c bus-$bus:"
    info_msg "$chip_addresses_final"
  done
}

# checks if chip address given is in the range of ligitimate chip addresses

check_if_ligitimate_chip_address() {
  local chip_address_to_check="$1"
  
  if [ -z "$chip_address_to_check" ]; then
    info_msg "chip address to check is not defined"
    return 1
  fi
  
  if_hexa "$chip_address_to_check"
  if [ "$?" -ne 0 ]; then
    return 1
  fi
  
  if_hexa_is_greater "$chip_address_to_check" "$min_chip_address"
  if [ "$?" -ne 0 ]; then
    info_msg "chip address should be in range form $min_chip_address to $max_chip_address"
    info_msg "$chip_address_to_check doesn't fit"
    return 1
  fi
  
  if_hexa_is_less "$chip_address_to_check" "$max_chip_address"
  if [ "$?" -ne 0 ]; then
    info_msg "chip address should be in range form $min_chip_address to $max_chip_address"
    info_msg "$chip_address_to_check doesn't fit"
    return 1
  fi
  
  return 0
}

# checks if given chip address is available in the given i2c bus
# parameters:
#	i2c_bus - integer (to get list of available i2c busses, use i2c_get_busses)
# 	chip_address - hexadecimal (to get list of available i2c addresses for a bus use i2cdetect -y -r <i2c_bus>)

check_if_chip_address_available_in_bus() {
  local i2c_bus="$1"
  local chip_address="$2"
  local copy_chip_address="$2"
  local chip_address_hexa_part=""
  local output=""
  
  copy_chip_address=$(echo $copy_chip_address | tr [A-Z] [a-z])
  chip_address_hexa_part=${copy_chip_address#$hexa_prefix_lc}
  
  output=$(i2cdetect -r -y $i2c_bus $chip_address $chip_address | awk -F":" '{print $2}' | grep $chip_address_hexa_part)
  if [ "$output" = "" ]; then
    info_msg "chip address $chip_address is not available for i2c bus $i2c_bus"
    return 1
  fi
  
  return 0
}

# checks if data address given is in the range of ligitimate data addresses

check_if_ligitimate_data_address() {
  local data_address_to_check="$1"
  
  if [ -z "$data_address_to_check" ]; then
    info_msg "data address to check is not defined"
    return 1
  fi
  
  if_hexa "$data_address_to_check"
  if [ "$?" -ne 0 ]; then
    return 1
  fi
  
  if_hexa_is_greater "$data_address_to_check" "$max_data_address"
  if [ "$?" -eq 0 ]; then
    info_msg "data address should be in less then $max_data_address"
    info_msg "$data_address_to_check doesn't fit"
    return 1
  else
    return 0
  fi
}

# checks if data value given is in the range of ligitimate values

check_if_ligitimate_value_to_write() {
  local value_to_check="$1"
  
  if [ -z "$value_to_check" ]; then
    info_msg "value to check is not defined"
    return 1
  fi
  
  # value can be both positive integer and hexadecimal
  if_positive_integer "$value_to_check"
  if [ "$?" -ne 0 ]; then
    # if value is not positive integer, check if it is hexadecimal
    if_hexa "$value_to_check"
    if [ "$?" -ne 0 ]; then
      # if value is neither positive integet nor hexadecimal - return fail
      info_msg "value to write $value_to_check is neither positive integer nor hexadecimal"
      return 1
    else
      # if value is hexadecimal, chech if it is in the range of legitimate values
      if_hexa_is_greater "$value_to_check" "$max_value_to_write_hexa"
      if [ "$?" -eq 0 ]; then
        info_msg "value to write should be in less then $max_value_to_write_hexa"
        info_msg "$value_to_check doesn't fit"
        return 1
      else
        return 0
      fi
    fi
  else
    # if value is positive integer, chech if it is in the range of legitimate values
    if [ $value_to_check -gt $max_value_to_write_int ]; then
      info_msg "value to write should be in less then $max_value_to_write_int"
      info_msg "$value_to_check doesn't fit"
      return 1
    else
      return 0
    fi
  fi
}

########	I2C MAIN FUNCTIONS

# returns value writen in given bus, address and register
# parameters:
#	i2c_bus - integer (to get list of available i2c busses, use i2c_get_busses)
# 	chip_address - hexadecimal (to get list of available i2c addresses for a bus use i2cdetect -y -r <i2c_bus>)
#	data_address - haxadecimal (in range 0x00 - 0xff)

i2c_read() {
  local i2c_bus="$1"
  local chip_address="$2"
  local data_address="$3"
  local result=""
  
  if [ -z "$i2c_bus" ]; then
    info_msg "i2c bus is not defined"
    return 1
  fi
  
  check_if_i2c_bus_exists "$i2c_bus"
  if [ "$?" -ne 0 ]; then
    info_msg "i2c bus $i2c_bus is not available"
    return 1
  fi
  
  if [ -z "$chip_address" ]; then
    info_msg "chip address is not defined"
    return 1
  fi
  
  check_if_ligitimate_chip_address "$chip_address"
  if [ "$?" -ne 0 ]; then
    info_msg "chip address $chip_address is not ligitimate"
    return 1
  fi
  
  check_if_chip_address_available_in_bus "$i2c_bus" "$chip_address"
  if [ "$?" -ne 0 ]; then
    return 1
  fi
  
  if [ -z "$data_address" ]; then
    info_msg "data address is not defined"
    return 1
  fi
  
  check_if_ligitimate_data_address $data_address
  if [ "$?" -ne 0 ]; then
    info_msg "data address $data_address is not ligitimate"
    return 1
  fi
  
  i2cset -y $i2c_bus $chip_address $data_address
  result=$(i2cget -y $i2c_bus $chip_address)
  
  info_msg "in i2c bus $i2c_bus chip adrress $chip_address data address $data_address is read $result"
  LAST_READ_RES="$result"
}

# writes value to given bus, address and register
# parameters:
#	i2c_bus - integer (to get list of available i2c busses, use i2c_get_busses)
# 	chip_address - hexadecimal (to get list of available i2c addresses for a bus use i2cdetect -y -r <i2c_bus>)
#	data_address - haxadecimal (in range 0x00 - 0xff)
#	value - haxadecimal (in range 0x00 - 0xff) or integer (in range 0 - 256)

i2c_write() {
  local i2c_bus="$1"
  local chip_address="$2"
  local data_address="$3"
  local value_to_write="$4"
  
  if [ -z "$i2c_bus" ]; then
    info_msg "i2c bus is not defined"
    return 1
  fi
  
  check_if_i2c_bus_exists "$i2c_bus"
  if [ "$?" -ne 0 ]; then
    info_msg "i2c bus $i2c_bus is not available"
    return 1
  fi
  
  if [ -z "$chip_address" ]; then
    info_msg "chip address is not defined"
    return 1
  fi
  
  check_if_ligitimate_chip_address "$chip_address"
  if [ "$?" -ne 0 ]; then
    info_msg "chip address $chip_address is not ligitimate"
    return 1
  fi
  
  check_if_chip_address_available_in_bus "$i2c_bus" "$chip_address"
  if [ "$?" -ne 0 ]; then
    return 1
  fi
  
  if [ -z "$data_address" ]; then
    info_msg "data address is not defined"
    return 1
  fi
  
  check_if_ligitimate_data_address "$data_address"
  if [ "$?" -ne 0 ]; then
    info_msg "data address $data_address is not ligitimate"
    return 1
  fi
  
  if [ -z "$value_to_write" ]; then
    info_msg "value to write is not defined"
    return 1
  fi
  
  check_if_ligitimate_value_to_write "$value_to_write"
  if [ "$?" -ne 0 ]; then
    info_msg "value to write $value_to_write is not ligitimate"
    return 1
  fi
  
  i2cset -y $i2c_bus $chip_address $data_address $value_to_write
  info_msg "in i2c bus $i2c_bus chip adrress $chip_address data address $data_address is written $value_to_write"
}

# writes value to given bus, address and register and then reads value from it 
# parameters:
#	i2c_bus - integer (to get list of available i2c busses, use i2c_get_busses)
# 	chip_address - hexadecimal (to get list of available i2c addresses for a bus use i2cdetect -y -r <i2c_bus>)
#	data_address - haxadecimal (in range 0x00 - 0xff)
#	value - haxadecimal (in range 0x00 - 0xff) or integer (in range 0 - 256)

i2c_write_and_read() {
  local i2c_bus="$1"
  local chip_address="$2"
  local data_address="$3"
  local value_to_write="$4"
  
  if [ -z "$i2c_bus" ]; then
    info_msg "i2c bus is not defined"
    return 1
  fi
  
  check_if_i2c_bus_exists "$i2c_bus"
  if [ "$?" -ne 0 ]; then
    info_msg "i2c bus $i2c_bus is not available"
    return 1
  fi
  
  if [ -z "$chip_address" ]; then
    info_msg "chip address is not defined"
    return 1
  fi
  
  check_if_ligitimate_chip_address "$chip_address"
  if [ "$?" -ne 0 ]; then
    info_msg "chip address $chip_address is not ligitimate"
    return 1
  fi
  
  check_if_chip_address_available_in_bus "$i2c_bus" "$chip_address"
  if [ "$?" -ne 0 ]; then
    return 1
  fi
  
  if [ -z "$data_address" ]; then
    info_msg "data address is not defined"
    return 1
  fi
  
  check_if_ligitimate_data_address "$data_address"
  if [ "$?" -ne 0 ]; then
    info_msg "data address $data_address is not ligitimate"
    return 1
  fi
  
  if [ -z "$value_to_write" ]; then
    info_msg "value to write is not defined"
    return 1
  fi
  
  check_if_ligitimate_value_to_write "$value_to_write"
  if [ "$?" -ne 0 ]; then
    info_msg "value to write $value_to_write is not ligitimate"
    return 1
  fi
  
  i2cset -y $i2c_bus $chip_address $data_address $value_to_write
  
  i2cset -y $i2c_bus $chip_address $data_address
  result=$(i2cget -y $i2c_bus $chip_address)
  
  info_msg "in i2c bus $i2c_bus chip adrress $chip_address data address $data_address is written $value_to_write, read: $result"
  LAST_READ_RES="$result"
}

# because of limitations of sh script, it;s impossible to return neither string not hexadecimal from function
# but messages printed by a function can be caught by the function caller
# i2c_read and i2c_write_and_read functions produce some info messages, so they can't sent the result string directly,
# but they write their read results to a global string variable, that can be obtained by calling the following function

i2c_get_read_result() {
  echo $LAST_READ_RES
}

