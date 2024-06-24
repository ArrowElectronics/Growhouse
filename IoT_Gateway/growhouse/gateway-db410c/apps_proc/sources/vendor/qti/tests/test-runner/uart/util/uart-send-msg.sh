#!/bin/sh
#Copyright (c) 2017, The Linux Foundation. All rights reserved.
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

####################################################################################################
# description: script sends txt message between UART connection points
# arg1 - from - a number representing UART connection point in the system
# arg2 - to - a number representing UART connection point in the system
####################################################################################################

################################# function definition ########################################## 
cleanup()
{
	echo "======> cleanup in $THIS_FILE_NAME <======="
	kill -9 "$(pidof cat)" 2>/dev/null

	rm "$UART_OUTPUT"
	rm "$EXPECTED_UART_OUTPUT"
	rm "$SEND_TO_UART_SCRIPT"
}
trap cleanup EXIT



#########################################
# arg1 - str to comapre
# arg2 - str to comapre
# arg3 - the element we are testing e.g num of new lins/words/bytes
#########################################
compareResults() 
{
	info_msg "========in compareResults======="
	info_msg "===arg1=$1=="
	info_msg "===arg2=$2=="
	info_msg "===arg3=$3=="


	if [ "$1" != "$2" ]; then
		error_msg "===== UART $UART_DEV$TO_UART_NUM not received correct number of $3 ====FAIL======"
#		echo "=====FAIL======="
#		exit 1
	else
		info_msg "======UART $UART_DEV$TO_UART_NUM successfully got correct number of $3========="
	fi
}

######################################## script execution #################################################
# defines general variables used in most tests
#. $(dirname $(readlink -f $0))/generally-used-variables.txt

# set result logging functions
. ../../lib/sh-test-lib

THIS_FILE_NAME=${0##*/}
FROM_UART_NUM=$1
TO_UART_NUM=$2
UART_DEV="/dev/ttyMSM"
TEST_MSG="this is a test message  to send between UART connections"
UART_OUTPUT="uart-output.txt"
EXPECTED_UART_OUTPUT="expected-uart-output.txt"
echo "$TEST_MSG" > $EXPECTED_UART_OUTPUT
info_msg "====TEST_MSG=$TEST_MSG====="

# wait for msg
info_msg "===UART $UART_DEV$TO_UART_NUM waiting to receive message from $UART_OUTPUT==="
#cat /dev/ttyMSM0 > uart0.log
cat "$UART_DEV$TO_UART_NUM" > "$UART_OUTPUT" &

# send msg
# echo "test message" > /dev/ttyMSM1
info_msg "==UART $UART_DEV$FROM_UART_NUM sending the message ===="
SEND_TO_UART_SCRIPT=send-to-uart.sh
echo "echo \"$TEST_MSG\" > $UART_DEV$FROM_UART_NUM" > $SEND_TO_UART_SCRIPT
chmod 777 $SEND_TO_UART_SCRIPT
./$SEND_TO_UART_SCRIPT

#analyze results
info_msg "=======$UART_OUTPUT content:=============="
#cat $UART_OUTPUT
TEMP="$(cat $UART_OUTPUT)"
info_msg "===DBUG:==UART_OUTPUT=$TEMP="

cat $UART_OUTPUT | grep -q "$TEST_MSG" || \
error_msg "=====$UART_DEV$TO_UART_NUM failed to receive msg======"

#(echo "=====$UART_DEV$TO_UART_NUM failed to receive msg======" && exit 1) || exit 1


# kill waiting process
kill -9 "$(pidof cat)" 2>/dev/null

UART_OUTPUT_COUNT="$( (wc $UART_OUTPUT) 2>&1)"
EXPECTED_UART_OUTPUT_COUNT="$( (wc $EXPECTED_UART_OUTPUT) 2>&1)"

info_msg "==============UART_OUTPUT_COUNT=$UART_OUTPUT_COUNT=================="
info_msg "==============EXPECTED_UART_OUTPUT_COUNT=$EXPECTED_UART_OUTPUT_COUNT=================="

COLOMN_NUM=1
PASS_CRITERIAS="newlines words bytes"
for PASSCRITERIA in $PASS_CRITERIAS; do
	OUTPUT="$(echo "$UART_OUTPUT_COUNT" | cut -d" " -f"$COLOMN_NUM")"
	EXPECTED_OUTPUT="$(echo "$EXPECTED_UART_OUTPUT_COUNT" | cut -d" " -f"$COLOMN_NUM")"

	if [ "$PASSCRITERIA" = "bytes" ]; then
		EXPECTED_OUTPUT=$((EXPECTED_OUTPUT+1))
	fi
	compareResults "$OUTPUT" "$EXPECTED_OUTPUT" "$PASSCRITERIA"
	COLOMN_NUM=$((COLOMN_NUM+1))
done

