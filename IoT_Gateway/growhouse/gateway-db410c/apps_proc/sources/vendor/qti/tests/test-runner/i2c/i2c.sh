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

# Additional info:
#
# i2c and GPIO test is platform depended. Test that suites to a certain board, probably wan't fit to another board.
# Now test is available for DB410c and DB820c. In order to add support for other boards the following steps have to be performed:
# 
# 1. The following line has to be modified:
#         if [ "$BOARD_NUM" = "410" -o "$BOARD_NUM" = "820" ]; then
#     Exapmle:
#         if [ "$BOARD_NUM" = "410" -o "$BOARD_NUM" = "820" -o "$BOARD_NUM" = "600" ]; then
#
# 2. Function that prepares file with GPIOs for i2c_0 test has to be added. Functions i2c_0_GPIO_make_APQ_GPIO_map_410_table and 
# i2c_0_GPIO_make_APQ_GPIO_map_820_table are such functions. Each of them can be used as basis. A good way to write such function 
# is to copy-paste one of them and make following changes:
#     a. Name. Example:
#          i2c_0_GPIO_make_APQ_GPIO_map_600_table
#     b. Numbers of GPIOs that correspond each letter from A to L. Such information can be obtained from schematics.
#
# 3. Variable that is a list of i2c buses connected to SX1508QBIULTRT chip on mezzanine has to be added.
#     Example:
#        i2c_other_600_bus_addresses="2 3"
#
# 4. elif statement has to be added to the following condition:
#      if [ "$BOARD_NUM" = "410" ]; then
#        local buses_list="$i2c_other_410_bus_addresses"
#      elif [ "$BOARD_NUM" = "820" ]; then
#        local buses_list="$i2c_other_820_bus_addresses"
#      else
#        info_msg "Unknown device: $BOARD_NUM"
#        report_fail "i2c-?"
#      fi
#
#     Example:
#      if [ "$BOARD_NUM" = "410" ]; then
#        local buses_list="$i2c_other_410_bus_addresses"
#      elif [ "$BOARD_NUM" = "820" ]; then
#        local buses_list="$i2c_other_820_bus_addresses"
#      elif [ "$BOARD_NUM" = "600" ]; then
#        local buses_list="$i2c_other_600_bus_addresses"
#      else
#        info_msg "Unknown device: $BOARD_NUM"
#        report_fail "i2c-?"
#      fi
#


# set result logging functions
. ../../lib/sh-test-lib
. ../../lib/set-result-dir.sh
. ../../lib/math/hexadecimals.sh
. ../../lib/math/integers.sh
. ../../lib/peripherals/i2c-lib.sh

# In order to turn GPIOs A-L on, they should be turn to output GPIOs (they are input by default)
# Register 0x0f is responsible for GPIOs A-H being input or output
# Register 0x0e is responsible for GPIOs I-L being input or output
# Register 0x11 containes values of output GPIOs A-H
# Register 0x10 containes values of output GPIOs I-L
#
# In the test, all GPIOs will be turned to output and turned off. Then they will be switched on and off one by one.
# After all GPIOS will be tested, they will receive there initial value and turned back to input GPIOs.

######################### i2c_0 #########################

i2c_0_chip_address="0x71"
i2c_0_GPIO_A_H_io_register="0x0f"
i2c_0_GPIO_I_L_io_register="0x0e"
i2c_0_GPIO_A_H_value_register="0x11"
i2c_0_GPIO_I_L_value_register="0x10"
i2c_0_GPIO_turn_output_mask="0x00"
i2c_0_GPIO_turn_input_mask="0xff"
i2c_0_turn_all_GPIOs_off_mask="0x00"

i2c_0_GPIO_turn_output() {
  i2c_write 0 $i2c_0_chip_address $i2c_0_GPIO_A_H_io_register $i2c_0_GPIO_turn_output_mask
  i2c_write 0 $i2c_0_chip_address $i2c_0_GPIO_I_L_io_register $i2c_0_GPIO_turn_output_mask
}

i2c_0_GPIO_turn_input() {
  i2c_write 0 $i2c_0_chip_address $i2c_0_GPIO_A_H_io_register $i2c_0_GPIO_turn_input_mask
  i2c_write 0 $i2c_0_chip_address $i2c_0_GPIO_I_L_io_register $i2c_0_GPIO_turn_input_mask
}

i2c_0_GPIO_turn_APQ_GPIO_input() {
  local curr_APQ_GPIO_num=""
  
  for n in A B C D E F G H I J K L
  do
    curr_APQ_GPIO_num=$(cat "$APQ_GPIO_map_file" | grep $n | awk -v ORS="" '{print $2}')
    echo $curr_APQ_GPIO_num > /sys/class/gpio/export
    echo "in" > /sys/class/gpio/gpio${curr_APQ_GPIO_num}/direction
  done
}

i2c_0_GPIO_cleanup_APQ_GPIO() {
  for n in A B C D E F G H I J K L
  do
    curr_APQ_GPIO_num=$(cat "$APQ_GPIO_map_file" | grep $n | awk -v ORS="" '{print $2}')
    echo $curr_APQ_GPIO_num > /sys/class/gpio/unexport
  done
}

i2c_0_GPIO_make_GPIO_masks_table() {
  GPIO_masks_file="GPIO_masks_table.txt"
  
  # cleanup
  eval rm "$GPIO_masks_file" > /dev/null 2>&1
  touch "$GPIO_masks_file"
  
  echo "A 0x01 0x00 0xfe 0xff" >> "$GPIO_masks_file"
  echo "B 0x02 0x00 0xfd 0xff" >> "$GPIO_masks_file"
  echo "C 0x04 0x00 0xfb 0xff" >> "$GPIO_masks_file"
  echo "D 0x08 0x00 0xf7 0xff" >> "$GPIO_masks_file"
  echo "E 0x10 0x00 0xef 0xff" >> "$GPIO_masks_file"
  echo "F 0x20 0x00 0xdf 0xff" >> "$GPIO_masks_file"
  echo "G 0x40 0x00 0xbf 0xff" >> "$GPIO_masks_file"
  echo "H 0x80 0x00 0x7f 0xff" >> "$GPIO_masks_file"
  echo "I 0x00 0x01 0xff 0xfe" >> "$GPIO_masks_file"
  echo "J 0x00 0x02 0xff 0xfd" >> "$GPIO_masks_file"
  echo "K 0x00 0x04 0xff 0xfb" >> "$GPIO_masks_file"
  echo "L 0x00 0x08 0xff 0xf7" >> "$GPIO_masks_file"
}

i2c_0_GPIO_make_APQ_GPIO_map_820_table() {
  APQ_GPIO_map_file="APQ_GPIO_map_table.txt"
  
  # cleanup
  eval rm "$APQ_GPIO_map_file" > /dev/null 2>&1
  touch "$APQ_GPIO_map_file"
  
  echo "A 80" 	>> "$APQ_GPIO_map_file"
  echo "B 29" 	>> "$APQ_GPIO_map_file"
  echo "C 125" 	>> "$APQ_GPIO_map_file"
  echo "D 24" 	>> "$APQ_GPIO_map_file"
  echo "E 62" 	>> "$APQ_GPIO_map_file"
  echo "F 494" 	>> "$APQ_GPIO_map_file"
  echo "G 10" 	>> "$APQ_GPIO_map_file"
  echo "H 8" 	>> "$APQ_GPIO_map_file"
  echo "I 25" 	>> "$APQ_GPIO_map_file"
  echo "J 26" 	>> "$APQ_GPIO_map_file"
  echo "K 23" 	>> "$APQ_GPIO_map_file"
  echo "L 133" 	>> "$APQ_GPIO_map_file"
}

i2c_0_GPIO_make_APQ_GPIO_map_410_table() {
  APQ_GPIO_map_file="APQ_GPIO_map_table.txt"
  
  # cleanup
  eval rm "$APQ_GPIO_map_file" > /dev/null 2>&1
  touch "$APQ_GPIO_map_file"
  
  echo "A 36" 	>> "$APQ_GPIO_map_file"
  echo "B 12" 	>> "$APQ_GPIO_map_file"
  echo "C 13" 	>> "$APQ_GPIO_map_file"
  echo "D 69" 	>> "$APQ_GPIO_map_file"
  echo "E 115" 	>> "$APQ_GPIO_map_file"
  echo "F 507" 	>> "$APQ_GPIO_map_file"
  echo "G 24" 	>> "$APQ_GPIO_map_file"
  echo "H 25" 	>> "$APQ_GPIO_map_file"
  echo "I 35" 	>> "$APQ_GPIO_map_file"
  echo "J 34" 	>> "$APQ_GPIO_map_file"
  echo "K 28" 	>> "$APQ_GPIO_map_file"
  echo "L 33" 	>> "$APQ_GPIO_map_file"
}

i2c_0_GPIO_analyze_writing_results() {
  local GPIO="$1"
  local on_or_off="$2"
  local curr_APQ_GPIO_num=""
  local curr_APQ_GPIO_output=""
  local fail_status=0
  
  if [ -z "$GPIO" ]; then 
    info_msg "GPIO isn't defined"
    return 1
  fi
  
  if [ ! -z `echo $GPIO | tr -d "[A-L]"` ]; then
    info_msg "Unknown GPIO: $GPIO"
    return 1
  fi
  
  if [ "$on_or_off" = "on" ]; then
    local expected_target_output=1
    local expected_other_output=0
  elif [ "$on_or_off" = "off" ]; then
    local expected_target_output=0
    local expected_other_output=1
  else
    info_msg "On/off input parameter has unknown value: \"$on_or_off\""
    return 1
  fi
  
  for n in A B C D E F G H I J K L
  do
    curr_APQ_GPIO_num=$(cat "$APQ_GPIO_map_file" | grep $n | awk -v ORS="" '{print $2}')
    curr_APQ_GPIO_output=$(cat /sys/class/gpio/gpio${curr_APQ_GPIO_num}/value)
    
    if [ "$n" = "$GPIO" ]; then
      if [ ! "$curr_APQ_GPIO_output" = "$expected_target_output" ]; then
        fail_status=1
      fi
    else
      if [ ! "$curr_APQ_GPIO_output" = "$expected_other_output" ]; then
        fail_status=1
      fi
    fi
  done
  
  if [ $fail_status -ne 0 ]; then
    report_fail "writing_${expected_target_output}_to_GPIO_${GPIO}"
  else
    report_pass "writing_${expected_target_output}_to_GPIO_${GPIO}"
  fi
}

i2c_0_GPIO_switch_on_off_test() {
  local on_or_off="$1"
  local mask_A_H=""
  local mask_I_L=""
  local result=""
  
  if [ "$on_or_off" = "on" ]; then
    info_msg "Starting switch GPIOs On test"
  elif [ "$on_or_off" = "off" ]; then
    info_msg "Starting switch GPIOs Off test"
  else
    info_msg "On/off input parameter has unknown value: \"$on_or_off\""
    return 1
  fi
  
  for l in A B C D E F G H I J K L
  do
    if [ "$on_or_off" = "on" ]; then
      mask_A_H=$(cat "$GPIO_masks_file" | grep $l | awk '{print $2}')
      mask_I_L=$(cat "$GPIO_masks_file" | grep $l | awk '{print $3}')
    else
      mask_A_H=$(cat "$GPIO_masks_file" | grep $l | awk '{print $4}')
      mask_I_L=$(cat "$GPIO_masks_file" | grep $l | awk '{print $5}')
    fi
    
    i2c_write 0 "$i2c_0_chip_address" "$i2c_0_GPIO_A_H_value_register" "$mask_A_H"
    i2c_write 0 "$i2c_0_chip_address" "$i2c_0_GPIO_I_L_value_register" "$mask_I_L"
    
    i2c_0_GPIO_analyze_writing_results $l "$on_or_off"
  done
}

check_i2c_0_GPIO() {
  local i2c_0_GPIO_A_H_initial_value=""
  local i2c_0_GPIO_I_L_initial_value=""
  
  # preparing for test
  ## preparing board GPIOs
  i2c_0_GPIO_make_APQ_GPIO_map_${BOARD_NUM}_table
  i2c_0_GPIO_turn_APQ_GPIO_input
  
  ## preparing mezzanine GPIOs
  i2c_0_GPIO_turn_output
  
  ## saving initial mezzanine GPIOs value
  i2c_read 0 $i2c_0_chip_address $i2c_0_GPIO_A_H_value_register
  i2c_0_GPIO_A_H_initial_value=$(i2c_get_read_result)
  
  i2c_read 0 $i2c_0_chip_address $i2c_0_GPIO_I_L_value_register
  i2c_0_GPIO_I_L_initial_value=$(i2c_get_read_result)
  
  ## prepare table with switch on and off masks for every GPIO
  i2c_0_GPIO_make_GPIO_masks_table
  
  ## turning all GPIOs off
  i2c_write 0 $i2c_0_chip_address $i2c_0_GPIO_A_H_value_register $i2c_0_turn_all_GPIOs_off_mask
  i2c_write 0 $i2c_0_chip_address $i2c_0_GPIO_I_L_value_register $i2c_0_turn_all_GPIOs_off_mask
  
  # performing test
  i2c_0_GPIO_switch_on_off_test "on"
  i2c_0_GPIO_switch_on_off_test "off"
  
  # cleaning after test 
  ## returning to mezzanine GPIOs their initial values
  i2c_write 0 $i2c_0_chip_address $i2c_0_GPIO_A_H_value_register $i2c_0_GPIO_A_H_initial_value
  i2c_write 0 $i2c_0_chip_address $i2c_0_GPIO_I_L_value_register $i2c_0_GPIO_I_L_initial_value
  i2c_0_GPIO_turn_input
  ## returning board GPIOs their initial state
  i2c_0_GPIO_cleanup_APQ_GPIO
  
  rm $GPIO_masks_file
  rm $APQ_GPIO_map_file
  
  report_pass "i2c-0"
}

######################### other i2c #########################

i2c_other_820_bus_addresses="1 2"
i2c_other_410_bus_addresses="1"
i2c_other_chip_address="0x23"
i2c_other_io_register="0x07"
i2c_other_value_register="0x08"
i2c_other_turn_output_mask="0x00"
i2c_other_turn_input_mask="0xff"
i2c_other_turn_all_ios_off_mask="0x00"

i2c_other_turn_output() {
  local i2c_bus_num=$1
  i2c_write $i2c_bus_num $i2c_other_chip_address $i2c_other_io_register $i2c_other_turn_output_mask
}

i2c_other_turn_input() {
  local i2c_bus_num=$1
  i2c_write $i2c_bus_num $i2c_other_chip_address $i2c_other_io_register $i2c_other_turn_input_mask
}

i2c_other_make_IO_masks_table() {
  IO_masks_file="IO_masks_table.txt"
  
  # cleanup
  eval rm "$IO_masks_file" > /dev/null 2>&1
  touch "$IO_masks_file"
  
  echo "IO0 0x01 0xfe" >> "$IO_masks_file"
  echo "IO1 0x02 0xfd" >> "$IO_masks_file"
  echo "IO2 0x04 0xfb" >> "$IO_masks_file"
  echo "IO3 0x08 0xf7" >> "$IO_masks_file"
  echo "IO4 0x10 0xef" >> "$IO_masks_file"
  echo "IO5 0x20 0xdf" >> "$IO_masks_file"
  echo "IO6 0x40 0xbf" >> "$IO_masks_file"
  echo "IO7 0x80 0x7f" >> "$IO_masks_file"
}

i2c_other_switch_on_off_test() {
  local i2c_bus=$1
  local on_or_off="$2"
  local mask=""
  local result=""
  local writing_value=""
  
  if [ -z "$i2c_bus" ]; then
    info_msg "i2c bus is not defined"
    return 1
  fi
  
  if [ "$on_or_off" = "on" ]; then
    info_msg "Starting switch IOs On test"
    writing_value="1"
  elif [ "$on_or_off" = "off" ]; then
    info_msg "Starting switch IOs Off test"
    writing_value="0"
  else
    info_msg "On/off input parameter has unknown value: \"$on_or_off\""
    return 1
  fi
  
  for io in 0 1 2 3 4 5 6 7
  do
    if [ "$on_or_off" = "on" ]; then
      mask=$(cat "$IO_masks_file" | grep "IO$io" | awk '{print $2}')
    else
      mask=$(cat "$IO_masks_file" | grep "IO$io" | awk '{print $3}')
    fi
    
    i2c_write_and_read $b "$i2c_other_chip_address" "$i2c_other_value_register" "$mask"
    result=$(i2c_get_read_result)
    
    if [ "$mask" = "$result" ]; then
      report_pass "writing_${writing_value}_to_IO_${io}"
    else
      report_fail "writing_${writing_value}_to_IO_${io}"
    fi
  done
}

check_i2c_other() {
  local i2c_other_initial_value=""
    
  if [ "$BOARD_NUM" = "410" ]; then
    local buses_list="$i2c_other_410_bus_addresses"
  elif [ "$BOARD_NUM" = "820" ]; then
    local buses_list="$i2c_other_820_bus_addresses"
  else
    info_msg "Unknown device: $BOARD_NUM"
    report_fail "i2c-?"
  fi
  
  ## prepare table with switch on and off masks for every io
  i2c_other_make_IO_masks_table
  
  for b in $buses_list
  do
    # preparing for test
    ## preparing mezzanine
    i2c_other_turn_output $b
  
    ## saving initial mezzanine ios value
    i2c_read $b $i2c_other_chip_address $i2c_other_value_register
    i2c_other_initial_value=$(i2c_get_read_result)
  
    ## turning all ios off
    i2c_write $b $i2c_other_chip_address $i2c_other_value_register $i2c_other_turn_all_ios_off_mask
  
    # performing test
    i2c_other_switch_on_off_test $b "on"
    i2c_other_switch_on_off_test $b "off"
  
    # cleaning after test 
    ## returning to mezzanine ios their initial values
    i2c_write $b $i2c_other_chip_address $i2c_other_value_register $i2c_other_initial_value
    i2c_other_turn_input $b
    
    report_pass "i2c-$b"
  done
  
  rm $IO_masks_file
}

BOARD_NUM="$1"

if [ "$BOARD_NUM" = "410" -o "$BOARD_NUM" = "820" ]; then
  info_msg "Starting i2c test"
  check_i2c_0_GPIO
  check_i2c_other
else
  info_msg "Unknown device: $BOARD_NUM"
  report_fail "i2c test"
fi

