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



#receives $1 = name of test, $2 = result of test (0-success, 0< - fail, 0> - log msg). $3 = (-v)if verbose status, $4 - verbose err msg 
#writes parms + data and time to txt file

#if verbose err msg is empty it just exits without logging

TEST_NAME=$1
TEST_RESULT=$2
VERBOSE=$3 # -v small verbose msg under (48894 chars),  -vb - large msg
VERBOSE_ERRMSG=$4
VERBOSE_ERR_LINE_NO=$5
################ function definition #########################
deleteLogIfToBig () #deletes if log file largger then 10 Megabyte
{
	LOG_FILE_SIZE=$(ls -al | awk '/'"$VERBOSE_LOG_FILE_NAME"'/ {print $5}')

	if [ $LOG_FILE_SIZE -gt 17000000 ]; then
		rm $VERBOSE_LOG_FILE_NAME 2>/dev/null
	fi
}

#################### script execution ############################

VERBOSE_LOG_FILE_NAME=verbose-test-log.txt
touch $VERBOSE_LOG_FILE_NAME
deleteLogIfToBig

if [ $TEST_RESULT -eq 0 ]; then
	TEST_RESULT="PASS" 
	
elif [ $TEST_RESULT -gt 0 ]; then
	TEST_RESULT="FAIL"
else
	TEST_RESULT="LOG MSG"
fi	


if [ "$VERBOSE" = -v ]; then
	echo "date:$(date). file name:$TEST_NAME - "LOG MSG", ERRMSG=$VERBOSE_ERRMSG. errLineNum:$VERBOSE_ERR_LINE_NO" >> $VERBOSE_LOG_FILE_NAME
	
elif [ "$VERBOSE" = -vb ]; then
	echo "$VERBOSE_ERRMSG" >> $VERBOSE_LOG_FILE_NAME
else
	ADDITIONAL_INFO=$VERBOSE
	echo "date:$(date) $TEST_NAME - $TEST_RESULT		$ADDITIONAL_INFO" >> test-log.txt 

fi





#if [ -n "$VERBOSE_ERRMSG" ]; then

	

#else
			
#fi

#date:Sun Aug 28 16:14:43 IDT 2016 test-executor.sh - LOG MSG		-vb


