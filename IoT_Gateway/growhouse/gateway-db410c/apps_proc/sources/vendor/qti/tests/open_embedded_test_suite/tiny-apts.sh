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

#!/bin/bash
#author c_aziski (Alfred Ziskind)
#tiny-apts
#=========>  must be run as super user  <==========

#transfers files from workStation to target device
#running the test-job scripts on target device
#looging results


####################################### test execution ##################################################################
chmod a+xw * -R
find . -type f -name "*~" -delete
EXIT_ERR=1

PRINT_BEGIN="test-job scripts transfer to target device in progress..."
PRINT_END="tiny-apts finished. test results see test-log.txt, verbose err log file see: verbose-test-log.txt \nATTENTION- verbose-test-log.txt will be deleted when reached 10 Megabyte"

echo $PRINT_BEGIN
./file-transferor.sh || (echo $PRINT_END && echo "file transferor failed" && exit $EXIT_ERR) || exit $EXIT_ERR

./test-executor.sh

echo "-----------" >> verbose-test-log.txt
echo "-----------" >> test-log.txt
echo -e $PRINT_END




