#!/bin/sh

# Set ble advertise name as GH_gw_xx:xx:xx:xx:xx:xx
#MAC=$(ip link show eth0 | awk '/ether/ {print $2}')

# Get BLE MAC address from hcitool
MAC=$(hcitool dev | grep -o "[[:xdigit:]:]\{11,17\}")
/bin/echo ${#MAC}=$MAC

while [ ${#MAC} -lt 10 ]
do
	sleep 3
	/usr/bin/hciconfig hci0 up
	MAC=$(hcitool dev | grep -o "[[:xdigit:]:]\{11,17\}")
	/bin/echo ${#MAC}=$MAC
done

NAME="GH_gw_$MAC"
BLE_ADV_NAME=$(echo $NAME |  tr -d "\n" | hexdump -v -e '/1 "%02x"')

# 02-length : 01 - flags
#	      06 - Bit 1 : "LE General Discoverable Mode"
#		   Bit 2 : "BR/EDR Not Supported."
# 18-length : 09 - Complete Local Name
#	      $BLE_ADV_NAME - Ble advertise name

HEX_NAME="0201061809$BLE_ADV_NAME"

/bin/systemctl stop bluetooth

/usr/bin/btmgmt power off
/usr/bin/btmgmt bredr off
/usr/bin/btmgmt le on
/usr/bin/btmgmt advertising off
/usr/bin/btmgmt connectable on
/usr/bin/btmgmt -i hci0 add-adv -d $HEX_NAME 1
/usr/bin/btmgmt power on
