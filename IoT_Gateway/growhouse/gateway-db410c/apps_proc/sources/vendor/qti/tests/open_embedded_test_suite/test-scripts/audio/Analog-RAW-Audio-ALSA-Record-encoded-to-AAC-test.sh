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
# this test:
# 1. Records RAW audio via Analog microphone
# 2. Encodes to AAC 
# 3. plays audio msg to user on HDMI screen
# 4. Plays recorded AAC using gstreamer on *USB headset and checks for errors or warnings 
# * the recording sounds to low on HDMI. on USB headphones sounds louder

# needs to be connected to the DUT: 
# 1 - Analog Mic (#TODO add a connected analog Mic illustration )
# 2 - USB headset
# 3 - HDMI screen


date
echo "=============> Running Test: Analog-RAW-Audio-ALSA-Record-encoded-to-AAC-test.sh <==============="
echo

# defines general variables used in most tests
source $(dirname $(readlink -f $0))/../../util/generally-used-variables.txt

ALSA_LOG_FILE="alsa.log"
RECORD_FILE=Analog_audio_record_AAC_test.AAC

#install missing packeges with opkg
$UTIL_PATH/install-pkg.sh faac || (echo "failed to install required packages" && exit 1) || exit 1

# define audio Hardware configuration - parse USB card and device numbers to play in USB headphones (e.g. plughw:1,0)
PLAYBACK_HW_CARD=$(aplay -l | grep "USB Audio" | cut -d":" -f1 | cut -d" " -f2)
PLAYBACK_HW_DEVICE=$(aplay -l | grep "USB Audio" | cut -d":" -f2 | cut -d"," -f2 | cut -d" " -f3)
USB_HEADPHONES_HW_CONFIG=plughw:$PLAYBACK_HW_CARD,$PLAYBACK_HW_DEVICE
# HDMI configuration
HDMI_HW_CONFIG=plughw:0,0

echo "===== testing ======> USB_HEADPHONES_HW_CONFIG=$USB_HEADPHONES_HW_CONFIG ========"

# do some cleanup (kill and clean any remainders from previous runs)
echo "some cleanup & setup ..."
echo  
eval kill -9 $(pidof "aplay") &>/dev/null
eval kill -9 $(pidof "arecord") &>/dev/null
eval kill -9 $(pidof "gst-launch-1.0") &>/dev/null

$UTIL_PATH/kill-process-ids.sh pulseaudio
rm $ALSA_LOG_FILE &>/dev/null
rm $RECORD_FILE &>/dev/null
sleep 1
echo
echo "done cleanup & setup, running test..."

#configuring sound card
#routing the Tombak codec
#======================================
amixer -c 0 cset name='PRI_MI2S_RX Audio Mixer MultiMedia1' 1 &>/dev/null
amixer -c 0 cset name='RX1 MIX1 INP1' 'RX1' &>/dev/null
amixer -c 0 cset name='RX2 MIX1 INP1' 'RX2' &>/dev/null
amixer -c 0 cset name='RDAC2 MUX' 'RX2' &>/dev/null
#Turn on both sides of the headphones
#========================================
amixer -c 0 cset name='HPHL' 'Switch' &>/dev/null
amixer -c 0 cset name='HPHR' 'Switch' &>/dev/null
amixer -c 0 cset name='MI2S_RX Channels' 'Two' &>/dev/null
amixer cset iface=MIXER,name='RX1 MIX1 INP1' 'RX1' &>/dev/null
amixer cset iface=MIXER,name='RX2 MIX1 INP1' 'RX2' &>/dev/null
amixer cset iface=MIXER,name='RDAC2 MUX' 'RX2' &>/dev/null
amixer cset iface=MIXER,name='HPHL' 1 &>/dev/null
amixer cset iface=MIXER,name='HPHR' 1 &>/dev/null
amixer cset iface=MIXER,name='RX1 Digital Volume' 40 &>/dev/null
amixer cset iface=MIXER,name='RX2 Digital Volume' 40 &>/dev/null
#turn on secondary mic - for analog mic
amixer cset iface=MIXER,name='DEC1 MUX' 'ADC2' &>/dev/null
amixer cset iface=MIXER,name='ADC2 Volume' 90 &>/dev/null
amixer cset iface=MIXER,name='ADC2 MUX' 'INP3' &>/dev/null
sleep 1


# run pulseaudio
X &
sleep 2
su $USER_NAME -lc $UTIL_PATH/pulseaudio-run-daemon.sh

# play msg to user in USB headset to start speaking
COMMAND="aplay -l -L && aplay -vv $MSG_TO_USER -D $HDMI_HW_CONFIG"
echo "Running Command: $COMMAND"
eval $COMMAND &>> $ALSA_LOG_FILE

# executing arecord
COMMAND="arecord -vv -D plughw:0,2 -f cd -d 5 | faac - -o $RECORD_FILE"
echo "Running Command: $COMMAND"
eval $COMMAND &>> $ALSA_LOG_FILE

# check if the file to play appears in the file system
if [ -e "$RECORD_FILE" ]
then 
  echo File $RECORD_FILE found on the file system, Ok!
else
  echo ERROR: File $RECORD_FILE NOT found on file system
  echo  
  date
  exit 1
fi

# executing gstreamer - playing in USB headset
COMMAND="gst-launch-1.0 -v filesrc location=$RECORD_FILE ! decodebin ! audioconvert ! audioresample ! alsasink device='$USB_HEADPHONES_HW_CONFIG'"
echo "Running Command: $COMMAND"
eval $COMMAND &>> $ALSA_LOG_FILE
sleep 1

echo "====> ALSA_LOG_FILE=$(cat $ALSA_LOG_FILE)"
# test finished, post processing data
# look into the log file and search for any errors
ERROR_COUNT="$(cat $ALSA_LOG_FILE | grep -c "error")"
WARNING_COUNT="$(cat $ALSA_LOG_FILE | grep -c "arning")"
ERROR_COUNT=$(( $WARNING_COUNT + $ERROR_COUNT))

echo  
echo Number of Errors or Warnings: $ERROR_COUNT
echo  
if [ "$ERROR_COUNT" -gt "0" ]
then 
  RESULT=1
  echo "Errors found!.."
  cat $ALSA_LOG_FILE | grep "arning"
  cat $ALSA_LOG_FILE | grep "error"
  echo 
  echo " - - F A I L - - "
else
  RESULT=0
  echo " + + P A S S + + "
fi

# garbage cleaning 
eval kill -9 $(pidof "aplay") &>/dev/null
eval kill -9 $(pidof "arecord") &>/dev/null
eval kill -9 $(pidof "gst-launch-1.0") &>/dev/null
$UTIL_PATH/kill-process-ids.sh pulseaudio
rm $ALSA_LOG_FILE &>/dev/null
rm $RECORD_FILE &>/dev/null
sleep 2


echo
echo "=============> Finished Test: Analog-RAW-Audio-ALSA-Record-encoded-to-AAC-test.sh <==============="
echo

date
exit $RESULT


