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
# This test checks CPU hotplug feature by turning CPUs off, and turning them on afterwards
# This test requires Trustzone image that supports PSCI features


date
echo "=============> Running Test: PSCI hotplug test <==============="
echo   

RESULT=0

for i in {0..3};
do
		echo 0 > /sys/devices/system/cpu/cpu$i/online
		dmesg | grep "psci: CPU$i killed." >/dev/null 2>&1
		if [ $? -ne 0 ]; then
			echo "Error shutting down CPU$i"
			RESULT=1
		fi

		echo 1 > /sys/devices/system/cpu/cpu$i/online
    		dmesg | grep "CPU$i: Booted secondary processor" >/dev/null 2>&1
		if [ $? -ne 0 ]; then
			echo "Error turning on CPU$i"
			RESULT=1
		fi
		sleep 1
done


if [ $RESULT -ne "0" ];
then 
	echo " - - F A I L - - "
else
	echo " + + P A S S + + "
fi

echo  
echo "=============> Finished Test: PSCI hotplug test <============="
echo  

date
exit $RESULT
 




