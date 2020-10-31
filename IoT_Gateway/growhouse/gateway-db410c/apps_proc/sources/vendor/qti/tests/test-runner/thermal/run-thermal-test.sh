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

# DESCRIPTION
#   This script is responsable for controlling how "./thermal_test.sh" will run.
#   Controlling iterations and calculating DhrystoneScore Average of the test.

####################  function definition  ############################
usage() {
    echo "Usage: $0 [-l <lower-temperature-limit> -u <upper-temperature-limit> -n <number-of-dhrystone-runs>]" 1>&2
    exit 1
}

check_dhry_exists(){
    # Test if dhry file exists.
	if [ ! -e "${DHRY_PATH}" ]; then
		error_msg "===> ${DHRY_PATH} not found. Please check if the \'dhry\' file exists in the bin folder. Exiting test. <==="
	fi
}

check_parameters(){
	if [ -z "$LOWER_LIMIT" ] || [ -z "$UPPER_LIMIT" ] ; then
		info_msg "KPI parameters were not provided. Ignoring the KPI values."
		IS_USER_PASSED_KPI_ARGS=false
	else
		info_msg "KPI parameters were provided. Defining test values for KPI."
		IS_USER_PASSED_KPI_ARGS=true
	fi 

	if [ -z "$DHRY_ITERATIONS" ]; then
		info_msg "iterations of thermal_test NOT provided. using default=$ITER"
	else
		info_msg "iterations of thermal_test provided=$DHRY_ITERATIONS"
		ITER=$DHRY_ITERATIONS
	fi
}

# The function sum all columns of dhrystones file
sum_columns_log_file() {
  SUM_DHRYSTONE1=$(awk 'NR>2{s+=$3}END{print s}' "$DHRY_LOG_SHORT")
  SUM_DHRYSTONE2=$(awk 'NR>2{s+=$4}END{print s}' "$DHRY_LOG_SHORT")
  SUM_DHRYSTONE3=$(awk 'NR>2{s+=$5}END{print s}' "$DHRY_LOG_SHORT")
  SUM_DHRYSTONE4=$(awk 'NR>2{s+=$6}END{print s}' "$DHRY_LOG_SHORT")
}

################## Test run ############################################
# shellcheck disable=SC1091
. ../../lib/sh-test-lib
#. lib/set-result-dir.sh
. ../../lib/set-result-dir.sh
. ../../lib/common-tests-lib.sh
. ./lib/thermal-conf.sh

! check_root && error_msg "This script must be run as root"

# Read the parameters
while getopts "l:u:n:h:i:" o; do
	case "$o" in
	l) LOWER_LIMIT="${OPTARG}" ;;     # Lower limit for the temperature
	u) UPPER_LIMIT="${OPTARG}" ;;     # Upper limit for the temperature
	n) DHRY_ARG="${OPTARG}" ;;        # Number of dhrystone runs - given to dhryston as an argument
	i) DHRY_ITERATIONS="${OPTARG}" ;; #number of thermal_test.sh iterations. if not given uses the default in thermal-conf.sh
	h) usage ;;                       # Shows help
	*) usage ;;
	esac
done
check_parameters

# Test if dhry file exists. exits script if not
check_dhry_exists
info_msg "Test Configuration:"
info_msg "Lower temperature limit = ${LOWER_LIMIT}"
info_msg "Upper temperature limit  = ${UPPER_LIMIT}"
info_msg "Max overheating temperature = ${MAX_TEMP}"
info_msg "dhrystone argument = ${DHRY_ARG}"
info_msg "test iterations = $DHRY_ITERATIONS"

echo "$TIME_ITERATION" > "${TIME_ITERATION_TEMP}"

# Kill dhry execution whenever the script finishes.
trap "kill_process $DHRY_PATH" EXIT

# kill old dhry processes
kill_process "$DHRY_PATH"

printf "%s %s %s %s %s %s %s\\n" "Time" "Zone0Temp" "Zone1Temp" "CPU0Freq" "CPU1Freq" "CPU2Freq" "CPU3Freq" >> "$LOG_FILE"
printf "%s %s %s %s %s %s %s\\n" "Start" "End" "DhrystoneScore1" "DhrystoneScore2" "DhrystoneScore3" "DhrystoneScore4" "DeltaTime" >> "$DHRY_LOG_SHORT"

info_msg "Starting Thermal Test"
COUNT=1
INIT_ITERATION_TIME=$(date)
while [ "$COUNT" -le "$ITER" ]; do

  info_msg " -------------------------------- "
  THERMAL_ITERATION_START_TIME=$(date +"%T")
  info_msg "Iteration: Running $COUNT of $ITER"
  info_msg "Start time: $THERMAL_ITERATION_START_TIME"

  ./thermal_test.sh -l "${LOWER_LIMIT}" -u "${UPPER_LIMIT}" -n "${DHRY_ARG}" -k "${IS_USER_PASSED_KPI_ARGS}" -c "${COUNT}"

  COUNT=$((COUNT+1))
  info_msg "End time: $(date)"
done

END_TEST_TIME=$(date)
info_msg " -------------------------------- "
info_msg " "
info_msg "Start Time (entire test): $INIT_ITERATION_TIME"
info_msg "End Time (entire test): $END_TEST_TIME"
info_msg "Test results are on target device at dir=$OUTPUT"
info_msg " "
sum_columns_log_file
info_msg "===> Dhrystone test results: <==="
if [ "$ITER" -ne 1 ]
then
    # If the number of iterations is bigger than 1, the average of each dhrystone score and
    # the average of averages will be calculated.
    # These averages will ignore the first iteration of the test.
    # This is the same value that can be seen on the spreadsheet.

    ITER_AVG=$((ITER-1))
    SUM_DHRYSTONE1_AVG=$(awk "BEGIN {print $SUM_DHRYSTONE1/$ITER_AVG}")
    SUM_DHRYSTONE2_AVG=$(awk "BEGIN {print $SUM_DHRYSTONE2/$ITER_AVG}")
    SUM_DHRYSTONE3_AVG=$(awk "BEGIN {print $SUM_DHRYSTONE3/$ITER_AVG}")
    SUM_DHRYSTONE4_AVG=$(awk "BEGIN {print $SUM_DHRYSTONE4/$ITER_AVG}")

    AVERAGE=$(awk "BEGIN {print (($SUM_DHRYSTONE1_AVG + $SUM_DHRYSTONE2_AVG + $SUM_DHRYSTONE3_AVG + $SUM_DHRYSTONE4_AVG) / 4) }")
    info_msg "The DhrystoneScore1 Average is $SUM_DHRYSTONE1_AVG"
    info_msg "The DhrystoneScore2 Average is $SUM_DHRYSTONE2_AVG"
    info_msg "The DhrystoneScore3 Average is $SUM_DHRYSTONE3_AVG"
    info_msg "The DhrystoneScore4 Average is $SUM_DHRYSTONE4_AVG"
    info_msg "The DhrystoneScore Average of the 4 last averages is $AVERAGE"
else
    # If the number of iterations is 1, the average of the four dhrystone scores
    # will be calculated.
    AVERAGE=$(awk "BEGIN {print (($SUM_DHRYSTONE1_AVG + $SUM_DHRYSTONE2_AVG + $SUM_DHRYSTONE3_AVG + $SUM_DHRYSTONE4_AVG) / 4) }")
    info_msg "The DhrystoneScore Average of the test is $AVERAGE"
fi
info_msg "=============> Finished Test: Thermal (Dhrystone)  <============== "
info_msg " "
