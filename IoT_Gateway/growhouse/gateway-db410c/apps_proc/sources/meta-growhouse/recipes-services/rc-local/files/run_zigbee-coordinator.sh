#!/bin/sh
if [ -e /dev/ttyMSM1 ]
then
	/usr/sbin/zigbeeCoordinator -b 115200 -f x -p /dev/ttyMSM1
else
	echo "NCP Device is not available"
fi
