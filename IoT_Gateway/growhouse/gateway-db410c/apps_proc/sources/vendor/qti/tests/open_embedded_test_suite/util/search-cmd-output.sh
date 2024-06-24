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


#!/bin/sh
#testing presence of all search items in a command output
#fails if at least one item missing, when fails logges the testing file and the missing item and returns 1
#on success return 0


# defines general variables used in most tests
source "$(dirname $(readlink -f $0))/generally-used-variables.txt"

THIS_FILE_NAME=$0
COMMAND="$1"
SEARCH_ITEMS="$2" 
LIST_OF_ITEMS_TO_SEARCH_IN=$(eval $COMMAND)
TEST_FILE_NAME=$3

LOGGER_PATH=$UTIL_PATH
FAIL=1

TRUE="true"
FALSE="false"
EMPTY_STR=""

IS_FOUND=$FALSE
MISSING_ITEMS=$EMPTY_STR

for I in $SEARCH_ITEMS
do
	for j in $LIST_OF_ITEMS_TO_SEARCH_IN
	do
		if [ "$I" = "$j" ]
		then
			IS_FOUND=$TRUE
			break
		fi
		IS_FOUND=$FALSE
	done
			
	if [ "$IS_FOUND" = "$FALSE" ]
	then
		MISSING_ITEMS="$MISSING_ITEMS $I"
	fi
done


if [ "$MISSING_ITEMS" = "$EMPTY_STR" ]
then
	exit 0
else
	$LOGGER_PATH/logger.sh $THIS_FILE_NAME $FAIL -v "module called from test file:$TEST_FILE_NAME. missing search items are: $MISSING_ITEMS" 45
	exit 1
fi
