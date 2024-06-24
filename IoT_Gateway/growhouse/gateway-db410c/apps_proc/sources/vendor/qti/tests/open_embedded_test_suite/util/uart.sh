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
# 


PASSORFAIL=3
FIRST=$1
SEC=$2

UTIL_PATH="$(dirname $(readlink -f $0))"
INPUT="hello first terminal I'm second"

#first terminal
touch fromsecterminal.txt
touch sec.txt
echo $INPUT > sec.txt
cat /dev/ttyMSM$SEC > fromsecterminal.txt &
#second terminal
eval busybox sh $UTIL_PATH/uartsecter.sh $FIRST 

#first terminal
OUT="$( (cat fromsecterminal.txt) 2>&1)"
# check output from second terminal
echo "$OUT" | grep "$INPUT" || (echo "failed to receive input from second terminal" && exit 1) || exit 1

WC="$( (wc sec.txt fromsecterminal.txt) 2>&1)"
echo "$WC"
export PSTTY="$( (ps | grep "cat /dev/ttyMSM" | sed -n '1p' | awk '{print $1}' ) 2>&1)"
OUT="$( (kill -9 $PSTTY) 2>&1)"

LINE1="$( (wc sec.txt fromsecterminal.txt | sed -n '1p') 2>&1)"
LINE2="$( (wc sec.txt fromsecterminal.txt | sed -n '2p') 2>&1)"
FIRSTIN1="$( ( echo $LINE1 | awk '{print $1}') 2>&1)"
FIRSTIN2="$( ( echo $LINE2 | awk '{print $1}') 2>&1)"

if [ "$FIRSTIN1" != "$FIRSTIN2" ]; then
	rm fromsecterminal.txt
	rm sec.txt	
	exit $PASSORFAIL	
fi
PASSORFAIL=$((PASSORFAIL-1))

SECIN1="$( ( echo $LINE1 | awk '{print $2}') 2>&1)"
SECIN2="$( ( echo $LINE2 | awk '{print $2}') 2>&1)"
if [ "$SECIN1" != "$SECIN2" ]; then
	rm fromsecterminal.txt
	rm sec.txt	
	exit $PASSORFAIL	
fi
PASSORFAIL=$((PASSORFAIL-1))

THIN1="$( ( echo $LINE1 | awk '{print $3}') 2>&1)"
THIN2="$( ( echo $LINE2 | awk '{print $3}') 2>&1)"
RESULT="$( (echo $THIN2 $THIN1 | awk '{print $1 - $2}') 2>&1)"

if [ "$RESULT" != 1 ]; then
	rm fromsecterminal.txt
	rm sec.txt	
	exit $PASSORFAIL	
fi
PASSORFAIL=$((PASSORFAIL-1))



rm fromsecterminal.txt
rm sec.txt
echo $PASSORFAIL
exit $PASSORFAIL


