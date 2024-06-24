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
# sending from 5  to 13
# sending from 11 to 7

# testing UART by sending and recieving signals in predefiend connection points
# IMPORTANT!! before running test connect gpio points as ilustrated:
# Ilustaration of connected gpio points. 
# symbol key:
#	J8 - this is the name of the gpio panel (printed on the board next to the panel)
#	*  - a white dot pointing to the the begining of the first gpio point (printed on the board next to the panel)
#	@ - connected gpio point
#	=== - connection between two gpio points

#	J8	#   #   #   #   #   #   #   #   #   #   #   #   #   #   #   #   #   #   #   #
#	*	#   #   @   @   #   @   @   #   #   #   #   #   #   #   #   #   #   #   #   #
#   			|   |=======|	|
#                       |===============|



echo "=============> Running Test: uart_test.sh <=============="
echo  
date

# defines general variables used in most tests
source $(dirname $(readlink -f $0))/../../util/generally-used-variables.txt

export OUTPUT="$( (stty -F /dev/ttyMSM0 -g) 2>&1)"
stty  -F /dev/ttyMSM1 $OUTPUT


$UTIL_PATH/uart.sh 0 1 || (echo "=====> uart.sh failed <====" && exit 1) || exit 1
$UTIL_PATH/uart.sh 1 0 || (echo "=====> uart.sh failed <====" && exit 1) || exit 1

echo "=============> finished Running Test: uart_test.sh <=============="
