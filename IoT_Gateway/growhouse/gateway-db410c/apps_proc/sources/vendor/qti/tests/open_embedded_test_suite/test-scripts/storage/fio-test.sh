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
#IMPORTANT! RUN TEST ONLY WITH USB mass storage INSIDE!!!
# runs fio-test.py from util directory
# receives as argument - USB mass storage device name
# run with USB mass storage device connected to DUT

echo "==================> starting fio-test.sh <========================="

# defines general variables used in most tests
source $(dirname $(readlink -f $0))/../../util/generally-used-variables.txt

USB_DEVICE=$1

#making sure fio package is installed on the DUT
$UTIL_PATH/install-pkg.sh fio >/dev/null \
|| (echo "===> fio package not installed on DUT <=====" && exit 1) || exit 1

echo "============> running fio-test.py  <=============="

RESULT="$(($UTIL_PATH/fio-test.py $USB_DEVICE) 2>&1)"

echo "=============> analyzing results <================"

echo $RESULT | grep "fio_512k_read_bandwidth: PASS" &>/dev/null \
|| (echo "======> fio-test.sh failed <======" && exit 1) || exit 1


echo "==================> successfully finished fio-test.sh <========================="


