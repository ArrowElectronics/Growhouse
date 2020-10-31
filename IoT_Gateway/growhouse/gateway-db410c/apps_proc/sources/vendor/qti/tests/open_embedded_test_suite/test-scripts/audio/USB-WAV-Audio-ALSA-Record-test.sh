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
# test records via USB microphone and plays it using ALSA aplay utility on USB headset

# connect USB headset with headphones and microphone to DUT


date
echo "=============> Running Test: USB-WAV-Audio-ALSA-Record-test.sh <==============="
echo

# defines general variables used in most tests
source $(dirname $(readlink -f $0))/../../util/generally-used-variables.txt

# define audio Hardware configuration - parse USB card and device numbers to play in USB headphones (e.g. plughw:1,0)
PLAYBACK_HW_CARD=$(aplay -l | grep "USB Audio" | cut -d":" -f1 | cut -d" " -f2)
PLAYBACK_HW_DEVICE=$(aplay -l | grep "USB Audio" | cut -d":" -f2 | cut -d"," -f2 | cut -d" " -f3)
#TODO: add a check to see the parsing is successful 
HW_CONFIG=plughw:$PLAYBACK_HW_CARD,$PLAYBACK_HW_DEVICE
echo "======> HW_CONFIG=$HW_CONFIG <====="

# check if the argument is supplied
if [ -z "$HW_CONFIG" ]
then
  echo  
  echo "Audio HW configuration not supplied. Typically this argument should be plughw:0,0 for internal sound card or plughw:1,0 for external such as USB sound card"
  echo
  date
  exit 1
fi


ALSA_LOG_FILE="alsa.log"

#record to file from usb for 5 seconds
FILE_TO_PLAY=USB_audio_record_test.wav

# do some cleanup (kill and clean any remainders from previous runs)
echo "some cleanup & setup ..."
echo  
eval kill -9 $(pidof "aplay") &>/dev/null
eval kill -9 $(pidof "arecord") &>/dev/null
rm $ALSA_LOG_FILE &>/dev/null
rm $FILE_TO_PLAY &>/dev/null
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

# play message to user in usb headphones to start recording
COMMAND="aplay -l -L && aplay -vv $MSG_TO_USER -D $HW_CONFIG"
echo "Running Command: $COMMAND"
eval $COMMAND &>> $ALSA_LOG_FILE

# executing arecord
COMMAND="arecord -vv -fdat $FILE_TO_PLAY -D hw:1,0 -d 5 -r 44100"
echo "Running Command: $COMMAND"
eval $COMMAND &>> $ALSA_LOG_FILE

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

# executing aplay
COMMAND="aplay -l -L && aplay -vv $FILE_TO_PLAY -D $HW_CONFIG"
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


echo
echo "=============> Finished Test: USB-WAV-Audio-ALSA-Record-test.sh <==============="
echo

date
exit $RESULT



