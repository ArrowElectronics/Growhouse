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
# stress-ng will stress test a computer system in various selectable ways. It was designed to exercise various physical subsystems of a computer as well as the various operating system kernel interfaces.



# defines general variables used in most tests
source $(dirname $(readlink -f $0))/../../util/generally-used-variables.txt

PASSORFAIL=0  # default- is success
echo "=============> Running Test Test: strng <=============="
echo  
date

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$UTIL_PATH


print_test_result()
{
	local test_case_id=$1
	local exit_code=$2
	if [ $exit_code -eq 0 ]; then
		echo "$test_case_id: pass"
	else
		echo "$test_case_id: fail"
		PASSORFAIL=$((PASSORFAIL+1))
	fi
}

duration=200
processor_number=$(grep processor /proc/cpuinfo | wc -l)
workloads="cpu io fork switch vm pipe yield hdd cache sock fallocate flock affinity timer dentry urandom sem open sigq poll"
workload_number=$(echo $workloads | wc -w)
sub_duration=$(( $duration / $workload_number ))
echo "CPU(s): $processor_number"
echo "Workloads to run: $workloads"
echo "Total stress_ng test duration: $duration seconds"
echo "Test duration for each workload: $sub_duration seconds"
count=1
for i in $workloads
do
	echo
	echo "[$count/$workload_number] Running $i workload..."
	if [ $i = "vm" ]; then
		# mmap 64M per vm process to avoid OOM, the default is 256M.
		$UTIL_PATH/stress-ng --$i $processor_number --vm-bytes 64m --timeout $sub_duration --metrics-brief --verbose 
	else
		$UTIL_PATH/stress-ng --$i $processor_number --timeout $sub_duration --metrics-brief --verbose 
	fi
	print_test_result $UTIL_PATH/stress-ng-$i $?
	count=$(( $count + 1 ))
done

echo "=============> Finished Test:  strng <=============="
echo  
date
exit $PASSORFAIL
