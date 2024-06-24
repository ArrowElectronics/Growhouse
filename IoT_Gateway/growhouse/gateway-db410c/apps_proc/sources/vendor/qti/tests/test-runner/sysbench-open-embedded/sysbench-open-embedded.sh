#!/bin/sh
#Copyright (c) 2017, The Linux Foundation. All rights reserved.
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


# description: SysBench is a modular, cross-platform and multi-threaded benchmark tool for 
#evaluating OS parameters that are important for a system under intensive load.

# set result logging functions
. ../../lib/sh-test-lib
. ../../lib/set-result-dir.sh
echo "======test========"
info_msg "========test========"
util_lib_path=$(realpath util)
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${util_lib_path}

test_type="cpu"
max_request=50000
duration=20

#check the sysbench presence
util/sysbench --version
exit_on_fail "check_sysbench_util_present"

for i in 1 2 3 4 5 6 7 8;
do
	thread_count=$((${i} * 2))
	sysbench_opt="--test=${test_type} --max-requests=${max_request} --num-threads=${thread_count} --max-time=${duration}"
	logfile="${OUTPUT}/sysbench-cpu-thread-cnt-${thread_count}.txt"
	util/sysbench ${sysbench_opt} run | tee "${logfile}"

  result_number_of_events=`cat $logfile | grep "total number of events" | awk {'print $5'}`

  info_msg "==> sysbench max_requests=$max_request duration=$duration\
  threads=$thread_count events=$result_number_of_events <=="
  
  if [ $result_number_of_events -gt 0 ]; then
    report_pass "sysbench_cpu_with_thread_count_${thread_count}" 
  else
    report_fail "sysbench_cpu_with_thread_count_${thread_count}" 
  fi

done
