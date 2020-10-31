#!/bin/bash
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

# DESCRIPTION
#   This script is the main file, it runs the real temperature test, warming-up the device and
#   collecting data to generate log files.
#
############## function definition ##########################################################
# The extract_dhrystone_short function extracts the dhrystone data from the dhry execution
# and puts on a file that its easier to import to the spreadsheet.
extract_dhrystone_short() {

    DHRYSTONE_SCORE_1=$(cat "$DHRY_LOG1" | grep "Dhrystones" | cut -d " " -f 25 | tail -1)
    DHRYSTONE_SCORE_2=$(cat "$DHRY_LOG2" | grep "Dhrystones" | cut -d " " -f 25 | tail -1)
    DHRYSTONE_SCORE_3=$(cat "$DHRY_LOG3" | grep "Dhrystones" | cut -d " " -f 25 | tail -1)
    DHRYSTONE_SCORE_4=$(cat "$DHRY_LOG4" | grep "Dhrystones" | cut -d " " -f 25 | tail -1)

    DHRY_LOG_END=$(date +%H:%M)

    # Convert the times to seconds from the Epoch
    DHRY_LOG_START_SECONDS=$(date +%s -d "${DHRY_LOG_START}")
    DHRY_LOG_END_SECONDS=$(date +%s -d "${DHRY_LOG_END}")
    DHRY_LOG_SECONDS_DIFF=$(expr "${DHRY_LOG_END_SECONDS}" - "${DHRY_LOG_START_SECONDS}")
    DHRY_DELTA_TIME=$(date +%H:%M:%S -ud "@${DHRY_LOG_SECONDS_DIFF}")

    printf "%s %s %s %s %s %s %s\\n" "$DHRY_LOG_START" "$DHRY_LOG_END" "$DHRYSTONE_SCORE_1" "$DHRYSTONE_SCORE_2" "$DHRYSTONE_SCORE_3" "$DHRYSTONE_SCORE_4" "$DHRY_DELTA_TIME" >> $DHRY_LOG_SHORT
}

############################ test run ##################################################
. ../../lib/sh-test-lib
#. ../../lib/set-result-dir.sh
. ./lib/thermal-conf.sh
. ../../lib/common-tests-lib.sh

# CPU Frequencies
CPU_FREQ0=0
CPU_FREQ1=0
CPU_FREQ2=0
CPU_FREQ3=0

# CPU temperatures
THERM_ZONE0=0
THERM_ZONE1=0

# Flag to indicates if the test has failed
IF_FAILED="false"

while getopts "l:u:n:k:c:" o; do
  case "$o" in
    l) LOWER_LIMIT="${OPTARG}" ;;           # Lower limit for the temperature
    u) UPPER_LIMIT="${OPTARG}" ;;           # Upper limit for the temperature
    n) DHRY_ARG="${OPTAG}" ;; 	            # Number of dhrystone runs - given to dhryston as an argument
    k) IS_USER_PASSED_KPI_ARGS="${OPTARG}" ;;
    c) ITERATION_COUNT="${OPTARG}" ;;
    *) usage ;;
  esac
done

echo "$DHRY_ARG" | "$DHRY_PATH" &>> "$DHRY_LOG1" &
echo "$DHRY_ARG" | "$DHRY_PATH" &>> "$DHRY_LOG2" &
echo "$DHRY_ARG" | "$DHRY_PATH" &>> "$DHRY_LOG3" &
echo "$DHRY_ARG" | "$DHRY_PATH" &>> "$DHRY_LOG4" &

info_msg "Initial TEMPERATURE_ZONE0 = $(cat /sys/devices/virtual/thermal/thermal_zone0/temp)"
info_msg "Initial TEMPERATURE_ZONE1 = $(cat /sys/devices/virtual/thermal/thermal_zone0/temp)"

DHRY_LOG_START=$(date +%H:%M)
#start test
while ps | grep dhry | grep -vq grep; do
    THERM_ZONE0=$(cat /sys/devices/virtual/thermal/thermal_zone0/temp)
    THERM_ZONE1=$(cat /sys/devices/virtual/thermal/thermal_zone1/temp)

    # Ensures the security of Thermal_test. If the temperature is bigger them the MAX_TEMP, the CPU is overheating.
    if [[ ( ($THERM_ZONE0 -gt $MAX_TEMP) || ($THERM_ZONE1 -gt $MAX_TEMP) )
    # If a IF_FAILED status was already reported for the same iteration, it wont report anymore.
    # It avoids duplicated failed results on the log file.
    && ( $IF_FAILED == "false" ) ]]
    then
        info_msg "Thermal test FAILED."
        info_msg "CPU overheating. Exiting the Test"
        info_msg "TEMPERATURE_ZONE0 = $THERM_ZONE0"
        info_msg "TEMPERATURE_ZONE1 = $THERM_ZONE1"

        report_fail "thermal-test-iteration=$ITERATION_COUNT"
        IF_FAILED="true"

        # Kill drhy before cooling down the board.
        kill_process "$DHRY_PATH"

        # When a CPU overheating happens, the test will exit of all subsequent tests because they will enter into this condition.
        # To prevent this from happening, the CPU stays 60 seconds cooling down.
        info_msg "Cooling down for 60 seconds"
        sleep 60

        # Finishes the test.
        break
    fi
    if [ "$IS_USER_PASSED_KPI_ARGS" == "true" ]; then

      # Verify if the temperature is within the range [$LOWER_LIMIT --- $UPPER_LIMIT]
      if [[ ( $ITERATION_COUNT -gt 1 ) && ! ( ( ( $THERM_ZONE0 -ge $LOWER_LIMIT ) && ( $THERM_ZONE0 -le $UPPER_LIMIT ) ) ||
      ( ( $THERM_ZONE1 -ge $LOWER_LIMIT ) && ( $THERM_ZONE1 -le $UPPER_LIMIT ) ) )
      # If a IF_FAILED status was already reported for the same iteration, it wont report anymore.
      # It avoids duplicated failed results on the log file.
      && ( $IF_FAILED == "false" ) ]]
      then
          info_msg "Thermal test FAILED."
          info_msg "The temperature is out of the range [$LOWER_LIMIT --- $UPPER_LIMIT]."
          info_msg "FAIL because of these temperatures: THERM_ZONE0=$THERM_ZONE0 and THERM_ZONE1=$THERM_ZONE1"

          report_fail "thermal-test-iteration=$ITERATION_COUNT"
          IF_FAILED="true"
      fi
    fi

    CPU_FREQ0=$(cat /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq)
    CPU_FREQ1=$(cat /sys/devices/system/cpu/cpu1/cpufreq/cpuinfo_cur_freq)
    CPU_FREQ2=$(cat /sys/devices/system/cpu/cpu2/cpufreq/cpuinfo_cur_freq)
    CPU_FREQ3=$(cat /sys/devices/system/cpu/cpu3/cpufreq/cpuinfo_cur_freq)

    TIME_ITERATION=$(cat "${TIME_ITERATION_TEMP}")

    printf "%s %s %s %s %s %s %s\n" "$TIME_ITERATION" "$THERM_ZONE0" "$THERM_ZONE1" "$CPU_FREQ0" "$CPU_FREQ1" "$CPU_FREQ2" "$CPU_FREQ3" >> "$LOG_FILE"

    # Get number of iterations
    let TIME_ITERATION+=1
    echo "$TIME_ITERATION" > "${TIME_ITERATION_TEMP}"

    # The test will capture data each second.
    sleep 1
done

info_msg "Final TEMPERATURE_ZONE0 = $THERM_ZONE0"
info_msg "Final TEMPERATURE_ZONE1 = $THERM_ZONE1"
extract_dhrystone_short

# If the test didn't fail (FAIL=0), then pass status is reported.
if [ $IF_FAILED = "true" ]; then
    report_fail "thermal-test-iteration=$ITERATION_COUNT"
else
    report_pass "thermal-test-iteration=$ITERATION_COUNT"
fi
