#Copyright (c) 2016, The Linux Foundation. All rights reserved.
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


#!/bin/bash
#executes test-jobs located on the target device

############################### function definition ####################################################################3
#recieves params: $1=TEST-JOB, $2=RESULT, $3=additional info
function print-result 
{
	if [ $2 -eq 0 ]
	then
		echo "$1 - PASS		$3"
	else
		echo "$1 - FAIL		$3"
	fi	
}

function get-test-job-args #$1 = test-job line from selectTestsWithARGS.txt
{
	local DELIMITER_NUM="$(echo $1 | awk -F':' '{print NF}')"
	if [ $DELIMITER_NUM -eq 1  ]; then
		echo "ERROR: in file selectTestsWithARGS.txt, if script has no arguments write \"NONE\" after colon" && exit 1
	fi
	echo $1 | cut -d":" -f"2-$DELIMITER_NUM" 
}

function get-test-job-file-name #$1 = test-job line from selectTestsWithARGS.txt
{
	echo $1 | cut -d":" -f1 
}

function cleneTargetDevCacheMem
{
	FREE_RET_VAL="$( (ssh $TARGET_ADDR free -h) 2>&1)" && ($WSTATION_LOGGER_PATH $THIS_FILE_NAME $LOG -v "FREE_RET_VAL before=$FREE_RET_VAL" $LINENO) || (($WSTATION_LOGGER_PATH $THIS_FILE_NAME $LOG -v "free failed" $LINENO) && return $EXIT_ERR) || return $EXIT_ERR

	SYNC_RET_VAL="$( (ssh $TARGET_ADDR sync) 2>&1)" || (($WSTATION_LOGGER_PATH $THIS_FILE_NAME $LOG -v "sync failed! SYNC_RET_VAL=$SYNC_RET_VAL" $LINENO) && return $EXIT_ERR) || return $EXIT_ERR

	CLEAN_RET_VAL="$( (ssh $TARGET_ADDR "echo 3 > /proc/sys/vm/drop_caches") 2>&1)" || (($WSTATION_LOGGER_PATH $THIS_FILE_NAME $LOG -v "clean cache failed! CLEAN_RET_VAL=$CLEAN_RET_VAL" $LINENO) && return $EXIT_ERR) || return $EXIT_ERR

	FREE_RET_VAL="$( (ssh $TARGET_ADDR free -h) 2>&1)" && ($WSTATION_LOGGER_PATH $THIS_FILE_NAME $LOG -v "FREE_RET_VAL after=$FREE_RET_VAL" $LINENO) || (($WSTATION_LOGGER_PATH $THIS_FILE_NAME $LOG -v "free failed!" $LINENO) && return $EXIT_ERR) || return $EXIT_ERR
}

function checkIfTestExists # $1 - test-script file name
{

	TEST_DIR_PATH=$(echo $1 | cut -d"/" -f1) 
	TEST_FILE_NAME=$(echo $1 | cut -d"/" -f2) 
	GREP_RET_VAL=$(ls $WSTATION_TEST_SCRIPTS_PATH/$TEST_DIR_PATH 2> /dev/null | grep $TEST_FILE_NAME) 
	
	if [ -z "$GREP_RET_VAL" ]; then
		GREP_RET_VAL=$(ls $WSTATION_TEST_SCRIPTS_PATH/../$TEST_DIR_PATH | grep $TEST_FILE_NAME)
		if [ -z "$GREP_RET_VAL" ]; then
		return $EXIT_ERR
		fi
	fi
}

function preExecutionSetup
{

	echo "=========> is this the first time you are running tiny-apts on current build? (yes/no)? <================"
	read FIRST_TIME_RUN

	# special commands that needs DUT to reboot
	if [ $FIRST_TIME_RUN = "yes" ]
	then		
		# install needed utilities
		ssh $TARGET_ADDR $DEVICE_UTIL_PATH/install-pkg.sh fio >/dev/null || (echo "failed to install required packages in test-executor.sh script" && exit 1) || exit 1
		
		ssh $TARGET_ADDR $DEVICE_UTIL_PATH/pre-execution-setup.sh 2>/dev/null
		echo "=========> Please unplug power from DUT and plug it back <================"
		echo "======= wait for 1 Min, or until the DUT is fully brought up ============="
		echo "=================== Press any key to continue ============================"
		read -n 1 -s
	fi
}

function IsTestLogConetntToBig
{
	RET_VAL=1

	# POSIX smallest allowable upper limit on argument length (all systems): 4096
	if [ $(echo "$TEST_LOG_CONTENT" | wc -c) -gt 4096 ]; then
		RET_VAL=0
	fi
	return $RET_VAL
}

function logLargeTestContent
{
	COUNT=1

	while read LINE; do
		if [ $COUNT -eq 1 ]; then	
			$WSTATION_LOGGER_PATH $THIS_FILE_NAME $LOG -v "$LINE" $LINENO
			COUNT=$((COUNT+1))
			continue
		fi
		$WSTATION_LOGGER_PATH $THIS_FILE_NAME $LOG -vb "$LINE" $LINENO

		COUNT=$((COUNT+1))
	done
}

#################### script execution #####################################
# defines general variables used in most tests
source "$(dirname $(readlink -f $0))/util/generally-used-variables.txt"

THIS_FILE_NAME=${0##*/}
FAIL=1
LOG=-2
EXIT_ERR=1

SUCCESSFUL_TEST_COUNTER=0

# Pre-execution operations
preExecutionSetup

#run all test-jobs files on target-device
echo "running tests on target device"
#ignores comments and new lines from selectTestsWithARGS.txt file
LIST_OF_TEST_JOBS=$(grep -v "#" $WSTATION_TEST_ARGS_PATH | grep -v "^$")
IFS='
'
for TEST_JOB_LINE in $LIST_OF_TEST_JOBS
do
	cleneTargetDevCacheMem
	$WSTATION_UTIL_PATH/check-ret-val-and-log.sh $? "function cleneTargetDevCacheMem failed" $WSTATION_LOGGER_PATH $THIS_FILE_NAME || exit $EXIT_ERR
	
	TEST_JOB_FILE_NAME="$(get-test-job-file-name "$TEST_JOB_LINE")"
	TEST_JOB_ARGS="$(get-test-job-args "$TEST_JOB_LINE")"
	checkIfTestExists $TEST_JOB_FILE_NAME
	$WSTATION_UTIL_PATH/check-ret-val-and-log.sh $? "TEST DOSN'T EXIST! TEST NAME:$TEST_JOB_FILE_NAME WRITEN IN FILE: $WSTATION_TEST_ARGS_PATH NOT FOUND IN DIR:$WSTATION_TEST_SCRIPTS_PATH. MAKE SURE IT'S NOT A TYPO" $WSTATION_LOGGER_PATH $THIS_FILE_NAME || exit $EXIT_ERR

	echo "$TEST_JOB_FILE_NAME args: $TEST_JOB_ARGS"
	#nullify test_string_ret_val.txt file (erase unrelated previous tests values)
	STRING_RET_VAL="$( (ssh $TARGET_ADDR "echo \"\" > test_string_ret_val.txt" ) 2>&1)"
	
	if [ "$(echo $TEST_JOB_FILE_NAME | cut -d"/" -f1)" = "$STATION_TEST_SCRIPTS_DIR" ]
	then
		#run tests on working station
		TEMP_RET_VAL_LOG=ret-val-log.temp
		$TEST_JOB_FILE_NAME $TEST_JOB_ARGS &> $TEMP_RET_VAL_LOG
		RESULT=$?
		TEST_LOG_CONTENT="$(cat $TEMP_RET_VAL_LOG)"
		rm $TEMP_RET_VAL_LOG &>/dev/null 
	else	
		#run tests on target device
		if [ "${TEST_JOB_FILE_NAME: -2}" = py ]
		then 
			TEST_LOG_CONTENT="$( (ssh $TARGET_ADDR python $TEST_SCRIPTS_PATH/"$TEST_JOB_FILE_NAME" $TEST_JOB_ARGS ) 2>&1)"
			RESULT=$?
		else
			TEST_LOG_CONTENT="$( (ssh $TARGET_ADDR $TEST_SCRIPTS_PATH/"$TEST_JOB_FILE_NAME" $TEST_JOB_ARGS ) 2>&1)"
			RESULT=$?

		fi
	fi
	#read string ret-val of test in file "test_string_ret_val.txt"
	STRING_RET_VAL="$( (ssh $TARGET_ADDR cat test_string_ret_val.txt ) 2>&1)"
	print-result "$TEST_JOB_FILE_NAME" $RESULT $STRING_RET_VAL
	$WSTATION_LOGGER_PATH $TEST_JOB_FILE_NAME $RESULT $STRING_RET_VAL

	IsTestLogConetntToBig && echo "$TEST_LOG_CONTENT" | logLargeTestContent \
	|| $WSTATION_LOGGER_PATH $TEST_JOB_FILE_NAME $LOG -v "$TEST_LOG_CONTENT" $LINENO

	TEST_COUNTER=$((TEST_COUNTER+1))
	(exit $RESULT) && SUCCESSFUL_TEST_COUNTER=$((SUCCESSFUL_TEST_COUNTER+1))

	echo "-------------------------------------------------------------------------------------"
	$WSTATION_LOGGER_PATH $THIS_FILE_NAME $LOG -v "-------------------------------------------------------------------------------------" $LINENO
done
IFS=$OLDIFS

SUMMERY="TEST SUMMERY: passed $SUCCESSFUL_TEST_COUNTER out of $TEST_COUNTER tests"
echo $SUMMERY
$WSTATION_LOGGER_PATH $SUMMERY $LOG






