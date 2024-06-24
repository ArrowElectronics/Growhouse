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
# plays gst for 14 hours
# Please connect HDMI to DUT


date
echo "=============> Running Test: Video-1080p-24fps-MOV-Audio-HDMI-stability-test.sh <==============="
echo

# defines general variables used in most tests
source $(dirname $(readlink -f $0))/../../util/generally-used-variables.txt

FILE_TO_PLAY="$CONTENT_PATH/$($GET_ENV_VARS VIDEO_FILE_1080P_24FPS_MOV)"

GST_PIPELINE="! qtdemux name=demuxer     demuxer. ! queue ! avdec_aac ! audioconvert ! audioresample ! alsasink device='hw:0,0'     demuxer. ! queue ! h264parse ! avdec_h264 ! glimagesink"

# 94 iterations of 9 MIN is 14.1 Hours
NUM_ITER=94
COUNTER=0

START_TIME=$(date +%s)

#run the whole video
while [ $COUNTER -lt $NUM_ITER ]
do
	echo "COUNTER=$COUNTER"
	echo "NUM_ITER=$NUM_ITER"
	
	# it plays for 540 seconds = 9 MIN. approximately the length of the movie file.
	$UTIL_PATH/play-gstreamer-and-check.sh $FILE_TO_PLAY "$GST_PIPELINE" 540 || (echo "utility  failed! - play-gstreamer-and-check.sh" && exit 1) || exit 1

	sleep 2

  COUNTER=$((COUNTER+1))
done

END_TIME=$(date +%s)
RUN_TIME=$(($END_TIME-$START_TIME))
EXIT_VAL=0

#14 hours in seconds is : 50400. suppose to be more than 14 hours
if [ $RUN_TIME -lt 50400 ]; then
	echo "test run less then 14 hours"
	EXIT_VAL=1
fi

echo "=============> RUN_TIME=$RUN_TIME  >======================"


echo "=============> test finished: Video-1080p-24fps-MOV-Audio-HDMI-stability-test.sh <==============="
echo

exit $EXIT_VAL


