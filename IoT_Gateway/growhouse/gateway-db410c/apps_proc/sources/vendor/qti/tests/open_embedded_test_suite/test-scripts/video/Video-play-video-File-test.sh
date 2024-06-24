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
# test args:
# arg 1 - file to play - variable name from "environmentVars.txt" file representing the video file
# e.g "VIDEO_FILE_720P_24FPS_MOV"
# arg 2 - audio output (e.g HDMI, analog, USB)



############################# functions definition  ###############################################
analog_mic_conf ()
{
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


	# Before analog playback
	amixer set PCM -- -2200
	amixer cset iface=MIXER,name='RX1 MIX1 INP1' 'RX1' &>/dev/null
	amixer cset iface=MIXER,name='RX2 MIX1 INP1' 'RX2' &>/dev/null
	amixer cset iface=MIXER,name='RDAC2 MUX' 'RX2' &>/dev/null
	amixer cset iface=MIXER,name='HPHL' 1 &>/dev/null
	amixer cset iface=MIXER,name='HPHR' 1 &>/dev/null
	amixer cset iface=MIXER,name='RX1 Digital Volume' 80 &>/dev/null
	amixer cset iface=MIXER,name='RX2 Digital Volume' 80 &>/dev/null
}

parse_audio_output ()
{
	if [ $AUDIO_OUTPUT = "HDMI" ]; then
		AUDIO_OUTPUT="0,0"
	elif [ $AUDIO_OUTPUT = "analog" ]; then
		analog_mic_conf
		AUDIO_OUTPUT="0,1"
	elif [ $AUDIO_OUTPUT = "USB" ]; then
		# parse USB card and device numbers to play in USB headphones (e.g. plughw:1,0)
		local PLAYBACK_HW_CARD=$(aplay -l | grep "USB Audio" | cut -d":" -f1 | cut -d" " -f2)
		local PLAYBACK_HW_DEVICE=$(aplay -l | grep "USB Audio" | cut -d":" -f2 | cut -d"," -f2 | cut -d" " -f3)
		AUDIO_OUTPUT="$PLAYBACK_HW_CARD,$PLAYBACK_HW_DEVICE"
	else
		return 1
	fi
}
################################# test execution #####################################################
date
echo "=============> Running Test: Video-720p-24fps-MOV-Audio-HDMI-test.sh <==============="
echo

VIDEO_ENV_VARIABLE=$1

AUDIO_OUTPUT=$2

# defines general variables used in most tests
source $(dirname $(readlink -f $0))/../../util/generally-used-variables.txt

$UTIL_PATH/check-args-num.sh $# 2 || ( echo "wrong number of arguments" && exit 1 ) || exit 1

parse_audio_output || ( echo "========> ERROR: wrong argument!! - user must supply:\
\"USB\", \"analog\", \"HDMI\" strings as 2nd argument <========" && exit 1) || exit 1


FILE_TO_PLAY="$CONTENT_PATH/$($GET_ENV_VARS $VIDEO_ENV_VARIABLE)"

echo "====testing FILE_TO_PLAY=$FILE_TO_PLAY ========"

GST_PIPELINE="! qtdemux name=demuxer \
    demuxer. ! queue ! avdec_aac ! audioconvert ! audioresample ! alsasink device="hw:$AUDIO_OUTPUT" \
    demuxer. ! queue ! h264parse ! avdec_h264 ! glimagesink"

echo "==============> GST_PIPELINE=$GST_PIPELINE <================"


$UTIL_PATH/play-gstreamer-and-check.sh $FILE_TO_PLAY "$GST_PIPELINE" || (echo "utility  failed! - play-gstreamer-and-check.sh" && exit 1) || exit 1


echo "=============> test finished: Video-720p-24fps-MOV-Audio-HDMI-test.sh <==============="
echo
