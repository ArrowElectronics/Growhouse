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
# This tests runs glxgears tool and measures the FPS perfomance.
# The test expects input FPS argument to assess PASS/FAIL decision.


date
echo "=============> Running Test: glxgears FPS <==============="
echo   

MIN_FPS_THRESHOLD=$1
MAX_FPS_THRESHOLD=$2

source $(dirname $(readlink -f $0))/../../util/generally-used-variables.txt


$UTIL_PATH/check-args-num.sh $# 2 || MIN_FPS_THRESHOLD=1 && MAX_FPS_THRESHOLD=1000000


if [ -z "$MAX_FPS_THRESHOLD" ]; then
	echo "FPS test limits arguments are not properly given"
	date
	exit 3
fi

GLX_GEARS_LOG_FILENAME="glxgears.log"

# do some cleanup (kill X and glxgears if they run before)
echo "some cleanup & setup ..."
kill -9 $(pidof glxgears) &> /dev/null
kill -9 $(pidof X) &> /dev/null
rm $GLX_GEARS_LOG_FILENAME &> /dev/null
sleep 1
echo  
echo "done cleanup & setup, running test..."

# start the X server
X &> X.log &
sleep 2

# run glxgears for 10 seconds
glxgears -fullscreen -display :0 > $GLX_GEARS_LOG_FILENAME &

sleep 10

# some cleanup
kill -9 $(pidof glxgears)
kill -9 $(pidof X)

# assess the results
ACTUAL_FPS=$(cat glxgears.log | grep -o -E '[0-9]+\.[0-9]+ FPS' | grep -m1 -o -E '[0-9]+' | grep -m1 -o -E '[0-9]+')
echo actual FPS was $ACTUAL_FPS
echo required FPS should within $MIN_FPS_THRESHOLD and $MAX_FPS_THRESHOLD
echo 

RESULT=1
if [ "$ACTUAL_FPS" -lt "$MIN_FPS_THRESHOLD" ] 
then
	RESULT=1
	echo "- - FAIL - - low FPS performance"
elif [ "$ACTUAL_FPS" -gt "$MAX_FPS_THRESHOLD" ] 
then
	RESULT=2
	echo "- - FAIL - - too high FPS performance, maybe LCD not connected..."
else
	RESULT=0
	echo "+ + + P A S S + + +"
fi

echo  
echo "=============> Finished Test: glxgears FPS <=============="
echo  

date
exit $RESULT
 




