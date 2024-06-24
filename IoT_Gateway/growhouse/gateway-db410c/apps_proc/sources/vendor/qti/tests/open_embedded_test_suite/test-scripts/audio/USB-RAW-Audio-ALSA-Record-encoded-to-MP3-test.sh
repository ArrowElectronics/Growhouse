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
# 1. plays audio msg to user on USB headphones
# 2. Records RAW audio via USB microphone
# 3. Encodes to MP3 with lame
# 4. Plays recorded MP3 using gstreamer on USB headphones and checks for errors or warnings 

# needs to be connected to the DUT: 
# 2 - USB headset with microphone and headphones


date
echo "=============> Running Test: USB-RAW-Audio-ALSA-Record-encoded-to-MP3-test.sh <==============="
echo

# defines general variables used in most tests
source $(dirname $(readlink -f $0))/../../util/generally-used-variables.txt

ALSA_LOG_FILE="alsa.log"
RECORD_FILE=USB_audio_record_MP3_test.wav

# define audio Hardware configuration - parse USB card and device numbers to play in USB headphones (e.g. plughw:1,0)
PLAYBACK_HW_CARD=$(aplay -l | grep "USB Audio" | cut -d":" -f1 | cut -d" " -f2)
PLAYBACK_HW_DEVICE=$(aplay -l | grep "USB Audio" | cut -d":" -f2 | cut -d"," -f2 | cut -d" " -f3)
USB_HEADPHONES_HW_CONFIG=plughw:$PLAYBACK_HW_CARD,$PLAYBACK_HW_DEVICE
echo "======> USB_HEADPHONES_HW_CONFIG=$USB_HEADPHONES_HW_CONFIG <====="

# do some cleanup (kill and clean any remainders from previous runs)
echo "some cleanup & setup ..."
echo  
eval kill -9 $(pidof "aplay") &>/dev/null
eval kill -9 $(pidof "arecord") &>/dev/null
eval kill -9 $(pidof "gst-launch-1.0") &>/dev/null
rm $ALSA_LOG_FILE &>/dev/null
rm $RECORD_FILE &>/dev/null
sleep 1
echo
echo "done cleanup & setup, running test..."

#configuring sound card
#routing the Tombak codec
#======================================
amixer -c 0 cset name='PRI_MI2S_RX Audio Mixer MultiMedia1' 1
amixer -c 0 cset name='RX1 MIX1 INP1' 'RX1'
amixer -c 0 cset name='RX2 MIX1 INP1' 'RX2'
amixer -c 0 cset name='RDAC2 MUX' 'RX2'

#Turn on both sides of the headphones
#========================================
amixer -c 0 cset name='HPHL' 'Switch'
amixer -c 0 cset name='HPHR' 'Switch'
amixer -c 0 cset name='MI2S_RX Channels' 'Two'

amixer cset iface=MIXER,name='RX1 MIX1 INP1' 'RX1'
amixer cset iface=MIXER,name='RX2 MIX1 INP1' 'RX2'
amixer cset iface=MIXER,name='RDAC2 MUX' 'RX2'
amixer cset iface=MIXER,name='HPHL' 1
amixer cset iface=MIXER,name='HPHR' 1
amixer cset iface=MIXER,name='RX1 Digital Volume' 40 
amixer cset iface=MIXER,name='RX2 Digital Volume' 40

# paly msg to user in usb headphones to start recording
COMMAND="aplay -l -L && aplay -vv $MSG_TO_USER -D $USB_HEADPHONES_HW_CONFIG"
echo "Running Command: $COMMAND"
eval $COMMAND &>> $ALSA_LOG_FILE

# executing arecord
COMMAND="arecord -vvv -f cd -t raw -D hw:1,0 -d 5 | lame --verbose --little-endian --signed -r -s 44.1 - $RECORD_FILE"
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

# executing gstreamer
COMMAND="gst-launch-1.0  filesrc location=$RECORD_FILE ! mad ! audioconvert ! audioresample ! alsasink device='$USB_HEADPHONES_HW_CONFIG'"
echo "Running Command: $COMMAND"
eval $COMMAND &>> $ALSA_LOG_FILE

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

# garbage clean
eval kill -9 $(pidof "aplay") &>/dev/null
eval kill -9 $(pidof "arecord") &>/dev/null
eval kill -9 $(pidof "gst-launch-1.0") &>/dev/null
rm $ALSA_LOG_FILE &>/dev/null
rm $RECORD_FILE &>/dev/null
sleep 1

echo
echo "=============> Finished Test: USB-RAW-Audio-ALSA-Record-encoded-to-MP3-test.sh <==============="
echo

date
exit $RESULT



