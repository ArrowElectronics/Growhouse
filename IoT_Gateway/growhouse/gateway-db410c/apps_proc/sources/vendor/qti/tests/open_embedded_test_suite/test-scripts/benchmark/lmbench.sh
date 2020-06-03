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
# Suite of simple, portable benchmarks
# Compares different systems performance 



# defines general variables used in most tests
source $(dirname $(readlink -f $0))/../../util/generally-used-variables.txt

PASSORFAIL=2  # need to read begind and end
echo "=============> Running Test Test: lmbench <=============="
echo  
date
PASSORFAIL=$((PASSORFAIL-1)) 
for i in $(seq 1 1000)
do
    echo "lat_ctx iteration number $i "
    $UTIL_PATH/lat_ctx  -s 64 2
    $UTIL_PATH/lat_ctx  -s 64 8
    $UTIL_PATH/lat_ctx  -s 64 16
    $UTIL_PATH/lat_ctx  -s 64 20
done
echo "=============> successfully completed Test: lmbench <=============="
echo  
date
PASSORFAIL=$((PASSORFAIL-1)) 


exit $PASSORFAIL
