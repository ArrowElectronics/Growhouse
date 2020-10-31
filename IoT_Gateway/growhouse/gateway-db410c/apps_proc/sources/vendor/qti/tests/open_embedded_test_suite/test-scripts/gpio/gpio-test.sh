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
#testing GPIO by sending and receiving signals in predefined connection points
#IMPORTANT!! before running test connect gpio points as illustrated:
#illustration of connected gpio points. 
#symbol key:
#	J8 - this is the name of the gpio panel (printed on the board next to the panel)
#	*  - a white dot pointing to the the beginning of the first gpio point (printed on the board next to the panel)
#	@ - connected gpio point
#	=== - connection between two gpio points

#	J8	#   #   #   #   #   #   #   @===@   #   #   @===@   @===@   @===@   #   #   #
#	*	#   #   #   #   #   #   #   #   #   #   #   @===@   @===@   @===@   #   #   #


################### function definition ##########################################
signal_to_apq() #$1=signal value(1/0).	$2=out_apq_name.	$3=in_apq_name.		$4=lineNum for logging msg.
{
	echo "$1" > $GPIO_PATH/$2/value
	READ_RES=$(cat $GPIO_PATH/$3/value)
	if [ $READ_RES != "$1" ]; then
		$UTIL_PATH/logger.sh $THIS_FILE_NAME $FAIL -v "writen $1, got: $READ_RES on: $2 to: $3" $4
		ERR_COUNT=$((ERR_COUNT+1))
	fi
}
########################## test run ###############################################################

# defines general variables used in most tests
source $(dirname $(readlink -f $0))/../../util/generally-used-variables.txt

THIS_FILE_NAME=$0
FAIL=1
GPIO_PATH=/sys/class/gpio

#allow Read Write permitions in export and unexport
chmod ugo+rw $GPIO_PATH/export
chmod ugo+rw $GPIO_PATH/unexport

#mapping gpio points
for IN_APQ in 13 24 28 113 12 25 33 ; do
	I=$((I+1))	#I= 1 .. 7
	eval IN_APQ_NAMES$I="gpio$IN_APQ"
	eval IN_APQ_NUMS$I=$IN_APQ
	eval IN_VAL_OLD$I=""  #creating vars to hold old values
done

MPP4=$(cat /sys/kernel/debug/pinctrl/200f000.spmi\:pm8916@0\:mpps@a000/gpio-ranges | grep 0 | cut -d"-" -f2 | cut -d"]" -f1 | cut -d" " -f2)

for OUT_APQ in 36 115 35 110 69 $MPP4 34 ; do 
	J=$((J+1))	#J= 1 .. 7
	eval OUT_APQ_NAMES$J="gpio$OUT_APQ"
	eval OUT_APQ_NUMS$J=$OUT_APQ
	eval OUT_VAL_OLD$J="" #creating vars to hold old values
done

INDEX_NUMS="1 2 3 4 5 6 7"
for index in $INDEX_NUMS ; do
#creating input, output gpio
	eval echo \$IN_APQ_NUMS$index > $GPIO_PATH/export
	eval echo \$OUT_APQ_NUMS$index > $GPIO_PATH/export
#getting maped gpio point
	IN_APQ_NAME=$(eval echo \$IN_APQ_NAMES$index)	
	OUT_APQ_NAME=$(eval echo \$OUT_APQ_NAMES$index)
#granting read write permitions to value, direction files
	chmod ugo+rw $GPIO_PATH/$IN_APQ_NAME/value
	chmod ugo+rw $GPIO_PATH/$IN_APQ_NAME/direction
	chmod ugo+rw $GPIO_PATH/$OUT_APQ_NAME/value
	chmod ugo+rw $GPIO_PATH/$OUT_APQ_NAME/direction
#saving old values
	eval IN_VAL_OLD$index=$(cat $GPIO_PATH/$IN_APQ_NAME/value)
	eval OUT_VAL_OLD$index=$(cat $GPIO_PATH/$OUT_APQ_NAME/value)
#testing loopbacks
	echo in > $GPIO_PATH/$IN_APQ_NAME/direction
	echo out > $GPIO_PATH/$OUT_APQ_NAME/direction
	signal_to_apq 1 $OUT_APQ_NAME $IN_APQ_NAME 71
	signal_to_apq 0 $OUT_APQ_NAME $IN_APQ_NAME 72
#restore gpios values and unexport  
	echo out > $GPIO_PATH/$IN_APQ_NAME/direction #turn IN gpio to OUT so you could write to it, so next line would work
	eval echo \$IN_VAL_OLD$index > $GPIO_PATH/$IN_APQ_NAME/value 
	eval echo \$OUT_VAL_OLD$index > $GPIO_PATH/$OUT_APQ_NAME/value
	eval echo \$IN_APQ_NUMS$index > $GPIO_PATH/unexport
	eval echo \$OUT_APQ_NUMS$index > $GPIO_PATH/unexport
done

exit $ERR_COUNT





