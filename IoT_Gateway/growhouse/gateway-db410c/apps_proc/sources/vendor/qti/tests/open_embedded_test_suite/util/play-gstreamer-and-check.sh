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
# this tests plays an H264 file using gstreamer pipeline
# this test also captures cpu temperature and cpu usage and prints this out.
# recievs: 
# arg1 = path to file to paly
# arg2 = gstreamr pipeline
# arg3 = duration in seconds to play file and monitor cpu


date
echo "=============> Running utility: play-gstreamer-and-check.sh <==============="
echo



FILE_TO_PLAY=$1
echo file "arg1 - file to play - $FILE_TO_PLAY"
GST_PIPELINE=$2
echo "arg2- GST_PIPELINE=$GST_PIPELINE"
TIMEOUT=$3
echo "arg3 - duration in seconds to play file and monitor cpu=$TIMEOUT"

if [ -z "$TIMEOUT" ]; then
	echo "arg3 is empty string. setting default value of 60 seconds to play file and monitor cpu"
	TIMEOUT=60
fi

UTIL_PATH="$(dirname $(readlink -f $0))"
$UTIL_PATH/setPerformanceModeCpu.sh

# check if the argument is supplied
if [ -z "$FILE_TO_PLAY" ]
then
  echo file-to-play argument not supplied
  echo
  date
  exit 1
fi

# check if the file to play appears in the file system
if [ -e "$FILE_TO_PLAY" ]
then 
  echo File $FILE_TO_PLAY found on the file system, Ok!
else
  echo ERROR: File $FILE_TO_PLAY NOT found on file system
  echo  
  date
  exit 1
fi

GSTREAMER_LOG_FILE="gstreamer_h264_playback.log"

# do some cleanup (kill and clean any remainders from previous runs)
echo "some cleanup & setup ..."
echo  
eval kill -9 $(pidof "gst-launch-1.0") &>/dev/null
rm $GSTREAMER_LOG_FILE &>/dev/null
kill -9 $(pidof "X") &>/dev/null
sleep 1
echo
echo "done cleanup & setup, running test..."

# activate X - windows system
export DISPLAY=:0
sleep 2
X &
pidof X &>/dev/null || (echo "X server failed to run" && exit 1) || exit 1
sleep 2
# change screen resolution 
HDMI_OUTPUT_NAME="$(xrandr -d :0 | grep connected | awk {'print $1'})"
# if xrandr fails to change resolution util keeps running
xrandr "-d" ":0" "--output" "$HDMI_OUTPUT_NAME" "--mode" "1920x1080" || ( echo "xrandr failed to change resolution" && exit 1 )
sleep 1

# executing gstreamer
COMMAND="gst-launch-1.0 -v filesrc location=$FILE_TO_PLAY $GST_PIPELINE"
echo "Running Command: $COMMAND"
eval $COMMAND &> $GSTREAMER_LOG_FILE &

sleep 3

# Check if gstreamer was launched ok, i.e. if we find its PID
GST_PID=$(pidof "gst-launch-1.0")
if [ -z "$GST_PID" ]
then
  echo 
  echo "ERROR: Could not start gstreamer"
  echo 
  date
  exit 1
fi


echo "playing and monitoring for $TIMEOUT seconds..."
echo

COUNTER=1
MAX_GST_CPU_USAGE=0
while [ "$COUNTER" -lt "$TIMEOUT" ]
do
  sleep 1
  echo "--> CPU statistic at second: $COUNTER <-------------------------------------"

  # CPU Usage in busybox is in the following format:
  # PID  PPID USER     STAT   VSZ %VSZ %CPU COMMAND
  # 
  # so, extract the CPU usage by extracting the percentages and getting the second value

  PERCENTAGES_TO_EXTRACT_FROM="$(top -b -n1 | grep gst-launch-1.0 | grep -o -E '[0-9]+%')"
  GST_CPU_USAGE="$(echo $PERCENTAGES_TO_EXTRACT_FROM | awk '{print $2}' | grep -o -E '[0-9]+')"
  echo "gst CPU usage: $GST_CPU_USAGE %"

  if [ "$GST_CPU_USAGE" -gt "$MAX_GST_CPU_USAGE" ]
  then
     MAX_GST_CPU_USAGE=$GST_CPU_USAGE
  fi

  echo "CPU Temperatures (degrees C): $(( $(cat /sys/class/thermal/thermal_zone0/temp) / 1000 )) \
$(( $(cat /sys/class/thermal/thermal_zone1/temp) / 1000 ))"

  echo "CPU CORE Frequencies (Hz):	\
$(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq)	\
$(cat /sys/devices/system/cpu/cpu1/cpufreq/scaling_cur_freq)	\
$(cat /sys/devices/system/cpu/cpu2/cpufreq/scaling_cur_freq)	\
$(cat /sys/devices/system/cpu/cpu3/cpufreq/scaling_cur_freq)"

  COUNTER=$((COUNTER+1))
done

echo 
echo "Maximum recorded gstreamer CPU usage is: $MAX_GST_CPU_USAGE %"

# test finished, killing gstreamer
kill -9 $(pidof "gst-launch-1.0") &>/dev/null
kill -9 $(pidof "X") &>/dev/null

# count the amount of the frame drop reports. Fail the test if there are too many (>2)
FRAME_DROP_REPORTS="$(cat $GSTREAMER_LOG_FILE | grep -c "A lot of buffers are being dropped")"
echo  
echo Number of Frame Drop Reports: $FRAME_DROP_REPORTS
echo  
if [ "$FRAME_DROP_REPORTS" -gt "2" ]
then 
  RESULT=2
  echo "Too many Frame Drops!.."
  echo 
  echo " - - F A I L - - "
else
  RESULT=0
  echo " + + P A S S + + "
fi



#writing result to file, to be read by test-executor.sh and used in log files
echo "max cpu usage:$MAX_GST_CPU_USAGE%" > test_string_ret_val.txt


echo
echo "=============> Finished utility: play-gstreamer-and-check.sh <==============="
echo

date
exit $RESULT



