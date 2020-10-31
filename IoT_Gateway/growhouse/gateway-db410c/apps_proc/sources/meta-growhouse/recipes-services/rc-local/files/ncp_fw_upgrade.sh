#!/bin/sh

if [ $# -ne 1 ]
then
	echo
	echo "Usage: sh $0 <firmware-file-path-location>"
	echo "Try Again..!"
	echo
	exit -1
fi

if [ -e $1 ]
then
	if [ -e /etc/ncp-fw-upgrade/ncp.py ]
	then
		#BTL detection GPIO low
		/bin/echo 0 > /sys/class/gpio/gpio36/value

		#Stop zigbee-coordinator service
		/bin/systemctl stop zigbee-coordinator

		#Reset MGM13P module
		/bin/echo 0 > /sys/class/gpio/gpio35/value

		#sleep
		/bin/sleep 1

		#Put out of reset MGM13P module
		/bin/echo 1 > /sys/class/gpio/gpio35/value

		/usr/bin/python2.7 /etc/ncp-fw-upgrade/ncp.py flash -p /dev/ttyMSM1 -f $1

		#BTL detction GPIO high
		/bin/echo 1 > /sys/class/gpio/gpio36/value

		#sleep
		/bin/sleep 0.5

		#Reset MGM13P module
		/bin/echo 0 > /sys/class/gpio/gpio35/value

		#sleep
		/bin/sleep 0.5

		#Put out of reset MGM13P module
		/bin/echo 1 > /sys/class/gpio/gpio35/value

		#sleep
		/bin/sleep 0.5

		#Start zigbee-coordinator service
		/bin/systemctl start zigbee-coordinator
	else
		echo "ncp.py python script not available"
	fi
else
	echo "Firmware file not present"
fi
