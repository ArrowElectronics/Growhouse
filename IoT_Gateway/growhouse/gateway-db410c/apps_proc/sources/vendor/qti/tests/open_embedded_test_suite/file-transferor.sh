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
#file transferor
#transfers test-job script files to target-device via scp
#operated by tiny-apts
#WSTATION is short for WORKING STATION

############################### function definition ################################################
createDirOnTargetDevice () # $1 - dir path
{
	STDERR="$( (ssh $TARGET_ADDR mkdir -p $1) 2>&1)" || 
(($WSTATION_LOGGER_PATH $THIS_FILE_NAME $LOG -v "$STDERR" $LINENO) && echo "ssh in file-transferor failed see verbose log"  && exit $EXIT_ERR) || exit $EXIT_ERR
}

copyToTargetDevice () # $1 - src dir.  $2 - dest dir    # copies all files from recieved dir to traget device
{
	STDERR="$( (rsync -avzP $1 $TARGET_ADDR:$2) 2>&1)" || (($WSTATION_LOGGER_PATH $THIS_FILE_NAME $LOG -v "$STDERR" $LINENO) && echo "rsync in file-transferor failed see verbose log"  && exit $EXIT_ERR) || exit $EXIT_ERR
}
################################## script execution #################################################

# defines general variables used in most tests
source "$(dirname $(readlink -f $0))/util/generally-used-variables.txt"

THIS_FILE_NAME=${0##*/}
FAIL=1
LOG=-3
EXIT_ERR=1

HOST_PATH="/root/.ssh/known_hosts"
ssh-keygen -f $HOST_PATH -R "$($GET_ENV_VARS TARGET_DEVICE_IP)" &>/dev/null
sleep 1
ssh-keyscan -H "$($GET_ENV_VARS TARGET_DEVICE_IP)" >> $HOST_PATH 2>/dev/null
#TODO: automate authentication from DUT to Helper and from HELPER to DUT


########## transfer test-jobs #################
#create dir for test-job scripts
createDirOnTargetDevice $TEST_SCRIPTS_PATH || exit $EXIT_ERR

#copy test-job scripts to target-device
copyToTargetDevice $WSTATION_TEST_SCRIPTS_PATH $BASE_PATH || exit $EXIT_ERR
######## transfer utils ##########

#create dir for utils to use from device
createDirOnTargetDevice $DEVICE_UTIL_PATH || exit $EXIT_ERR

#copy logger to util - so test-scripts could use logger on the device
cp logger.sh $WSTATION_UTIL_PATH
#copy environmentVars.txt to util so get-env-var-val-target.sh could use it to give tests-scripts access to environment variables
cp environmentVars.txt $WSTATION_UTIL_PATH || ( echo "failed to copy environmentVars.txt" && exit 1 ) || exit 1
#copy util files to target-device
copyToTargetDevice $WSTATION_UTIL_PATH $BASE_PATH || exit $EXIT_ERR

############ transfer content files ##################
#create dir for content files to be used in tests
createDirOnTargetDevice $CONTENT_PATH || (echo "failed to create dir for content files to be used in tests" && exit $EXIT_ERR) || exit $EXIT_ERR

#copy content files to target device
copyToTargetDevice $WSTATION_CONTENT_FILE_PATH $BASE_PATH || exit $EXIT_ERR

exit 0


