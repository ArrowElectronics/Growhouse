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
# SysBench is a modular, cross-platform and multi-threaded benchmark tool for evaluating OS parameters that are important for a system running a database under intensive load.



# defines general variables used in most tests
source $(dirname $(readlink -f $0))/../../util/generally-used-variables.txt

TIMES=8
PASSORFAIL=0 #default is success 


echo "=============> Running Test Test: sysbench <=============="
echo  
date

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$UTIL_PATH

TEST=${2-cpu}
MAX_REQUESTS=${3-50000}
DURATION=${4-20}
FILE_TEST_MODE=${5-seqrewr}

echo "TIMES IS $TIMES"
for i in $(seq 1 ${TIMES});
do
    t=$((${i} * 2));
    log_file="${TEST}-test-${t}-threads.log";
    opt="--test=${TEST} --max-requests=${MAX_REQUESTS} --num-threads=${t} --max-time=${DURATION}"
    [ "${TEST}" = "threadsR" ] && opt="${opt} --thread-locks=$((${t}/2))"
    [ "${TEST}" = "fileio" ] && opt="${opt} --file-test-mode=${FILE_TEST_MODE}"
    echo "Running sysbench ${opt} run" | tee ${log_file};
    $UTIL_PATH/sysbench ${opt} run | tee -a ${log_file};

    # parse log file & submit to test result
    sed -n -e '/Test execution summary\|General statistics/,/^$/p' ${log_file} | while read line;
    do
        id=$(echo ${line}|awk -F':' '{print $1}')
        val=$(echo ${line}|awk -F':' '{print $2}')
        if [ -n "${val}" ]; then
            u=$(echo ${val}|sed 's/[0-9\.]*//')
            v=$(echo ${val}|sed 's/\([0-9\.]*\)[a-zA-Z]*/\1/')
            # let 's' to be the default unit for time measurement
            [ -n "$(echo ${id}|grep 'time')" ] && [ -z "${u}" ] && u=s
            [ -n "${u}" ] && o="--units ${u} --measurement ${v}" || o="--measurement ${val}"
            echo "PASS: ${o}"  
	else
		PASSORFAIL=$((PASSORFAIL+1))	        
	fi
	
    done
done

echo "=============> Finished Test:  sysbench <=============="
echo  
date

exit $PASSORFAIL
