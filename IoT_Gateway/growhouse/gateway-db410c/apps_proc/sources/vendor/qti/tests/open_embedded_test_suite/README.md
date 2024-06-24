#tiny-apts documentation

acronym definition:
working station or WSTATON - pc that is the working station connected to the device under test
DUT - device/board under test
HELPER - the second board connected to the WSTATION for tests that require a second board
TBD - to be developed

working station requirements: 
the pc that is the working station connected to the DUT must be debian (e.g jessie) distribution or ubuntu


*** how to operate:
run only tiny-apts.sh!! this is the main execution file script that calls other scripts.

*** in test-scripts folder put only!!! test-job scripts that exit with 0 for success or higher than 0 for failure.

step by step:

1. make sure you can connect to your board via ssh as root without it prompting for password,
also from DUT to HELPER and from HELPER to DUT without prompting for password.
for instructions see http://www.linuxproblem.org/art_9.html


2. defining environment variables in environmentVars.txt:
	2.1) TARGET_DEVICE_IP - set to your board Ethernet interface IP address, so tiny-apts can connect to it.
	2.2) TARGET_DEVICE_USER - the user name on the DUT.
	2.3) HELPER_DEVICE_IP - set to second boards Ethernet interface IP address
	2.4) CONTENT_FILE_PATH - some tests (e.g audio, video) will require sound/video files to run, 
	here you set the full path of the directory on the working station containing these files 
	2.5) CLIENT_CONTENT_DIR - just the directory name of the content files directory. 
	these is the name that will appear on the DUT.
	2.6) content variables, set only the files name: SOUND_FILE_WAV, SOUND_FILE_MP3, SOUND_FILE_OGG, 
	VIDEO_FILE_1080P_24FPS_MOV, VIDEO_FILE_1080P_30FPS_MOV, VIDEO_FILE_1080P_60FPS_MP4, VIDEO_FILE_720P_24FPS_MOV
	2.7) optional variables - are meant for developers if there is a need to change current directories names in the file tree. 
	for just running/developing tests, they should not be altered
		

3. in file selectTestsWithARGS.txt you can select the tests you wish to run by uncommenting them

4. when tiny-apts is complete, see test-log.txt for a summary of pass/fail results and for a verbose output see verbose-test-log.txt


IMPORTANT GENERAL NOTE: all tests are written for and validated with busybox shell

*** list of tests below requires executables added to util directory or appropriate package installed  on the device

1. fio-test.sh requires fio-test.py - https://github.com/Linaro/test-definitions/blob/master/ubuntu/scripts/fio-test.py

2. ebiz.sh requires ebizzy - https://sourceforge.net/projects/ebizzy/

3. gettime.sh requires gettimeofday02 - https://github.com/linux-test-project/ltp/blob/master/testcases/kernel/syscalls/gettimeofday/gettimeofday02.c

4. hackbench.sh requires hackbench - https://github.com/linux-test-project/ltp/blob/master/testcases/kernel/sched/cfs-scheduler/hackbench.c

5. lmbench.sh requires lat_ctx - https://github.com/foss-for-synopsys-dwc-arc-processors/lmbench/blob/master/src/lat_ctx.c

6. lscpu.sh requires lscpu - https://layers.openembedded.org/layerindex/recipe/21404/

7. rtc.sh requirs rtc01 - https://github.com/linux-test-project/ltp/blob/master/testcases/kernel/device-drivers/rtc/rtc01.c

8. strng.sh requires stress-ng - https://github.com/ColinIanKing/stress-ng

9. sysbench.sh requires sysbench - https://github.com/akopytov/sysbench

Some benchmarks depends on these libraries, please add these lib files to util directory:

libapparmor.so.1

libboost_system.so.1.55.0

libboost_thread.so.1.55.0

libbsd.so.0

libibverbs.so.1

libmysqlclient.so.18

librados.so.2

librbd.so.1

librdmacm.so.1



OPERATING SYSTEM TOOLS IN USE:
a description of tools (like iozone, hostapd, etc) and their usage in each test.
purpose: in case your operating system miss some tools or uses the tools in a different way, here we show exactly how tiny-apts uses those tools

test-script file name:

*wifi-connect-open-security-test.sh
*wifi-connect-WPA2-PSK-security-test.sh
*wifi-soft-ap-connect-test.sh
	this test uses the following tools: 
		wpa_supplicant
		usage: wpa_supplicant -B -i <wifi device e.g: wlan0> -c <wpa supplicant configuration file>

		wpa_cli
		wpa_cli status

		ip
		ip route show

		ping
		ping -c <number of packeges to ping> <gateway IP to ping to>

*wifi-soft-ap-connect-test.sh
	this test uses the following tools: 
		iw
		iw list

		ifconfig
		ifconfig <wifi interface like wlan0> up <some static IP like 192.168.8.1> netmask 255.255.255.0

		hostapd
		hostapd -d -B <hostapd configuration file>

* sdcard-access-iozone-test.sh
	this test uses the following tools: 
		iozone
* flash-access-iozone-test.sh
	this test uses the following tools: 
		iozone
* random-access-flash-iozone-test.sh 
	this test uses the following tools: 
		iozone

* copyfiles-tests
	this test uses the following tools: 
		mount
		umount

