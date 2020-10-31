#!/bin/sh

TTY_NODE=/dev/ttyMSM0
LOG_FILE=/var/log/growhouse.log
NCP_STACK_VERSION="ncpStackVersion"
PUBLISH=/usr/bin/mosquitto_pub
SUBSCRIBE=/usr/bin/mosquitto_sub
PIPE=/tmp/testpipe
TEST_LOG_FILE=/home/root/test.log
EUI64=

BOARD_STARTUP_MESSAGE="linux.local ttyMSM0"
ZIGBEE_FLASH_TEST="ZigBee Coordinator Flash Test :"
ETHERNET_TEST="Ethernet Test :"
ZIGBEE_NETWORK_TEST="ZigBee Network Test :"
FACTORY_RESET_BUTTON_TEST="Factory Reset Button Test :"
ZIGBEE_RESET_BUTTON_TEST="ZigBee Reset Button Test :"
DEVICE_MAC_ADDRESS="Mac Address"

ZIGBEE_NETWORK_TIMEOUT=180
BUTTON_TIMEOUT=30
factoryResetProcess=1
HardwareReset=1

ethernetTest() {

	/bin/ping -c 5 -q $1
	ret=`echo $?`
	echo output == $ret >> $TEST_LOG_FILE
	if [ $ret -eq 0 ]
	then
		echo "Ethernet Working" >> $TEST_LOG_FILE
		echo "$ETHERNET_TEST PASS" > $TTY_NODE
	else
		echo "Ethernet is not working" >> $TEST_LOG_FILE
		echo "$ETHERNET_TEST FAIL" > $TTY_NODE
	fi
}

zigbeeFlashTest() {

	value=$(<$LOG_FILE)
	if echo "$value" | grep -q $NCP_STACK_VERSION
	then
		echo "Zigbee Coordinator flashed Successfully" >> $TEST_LOG_FILE
		# Get the value of EUI64 String
	#	EUI64=$(echo "$value" | grep -w "EUI64" | head -n 1 | cut -d" " -f8)
	#	echo Zigbee Coordinator eui64 String $EUI64  >> $TEST_LOG_FILE
		echo "$ZIGBEE_FLASH_TEST PASS" > $TTY_NODE
	else
		echo "Zigbee Coordinator flash Failed" >> $TEST_LOG_FILE
		echo "$ZIGBEE_FLASH_TEST FAIL" > $TTY_NODE
	fi
}

zigbeeNetworkTest() {

	value=$(<$LOG_FILE)
	# Get the value of EUI64 String
	EUI64=$(echo "$value" | grep -w "EUI64" | head -n 1 | cut -d" " -f8)
	echo Zigbee Coordinator eui64 String $EUI64  >> $TEST_LOG_FILE

	ZIGBEE_COMMAND=gw/$EUI64/commands
        echo "zigbee command == $ZIGBEE_COMMAND" >> $TEST_LOG_FILE

	count=1
	processExist=1

	# Subscribe for the Device joined topic
	$SUBSCRIBE -t gw/+/devicejoined -C 1 &
	# Get the process Id
	PID=$!
	echo "Process id === $PID"  >> $TEST_LOG_FILE

	# Send command to open ZigBee Coordinator network
	$PUBLISH -m '{"commands":[{"command":"plugin network-creator-security open-network","postDelayMs":100}]}' -t $ZIGBEE_COMMAND

	while [ $count -le $ZIGBEE_NETWORK_TIMEOUT ]; do
		echo $count
		if [ -n "${PID}" -a -d "/proc/${PID}" ]
		then
			echo "process exists" >> $TEST_LOG_FILE
		else
			processExist=0
			echo "process not exists" >> $TEST_LOG_FILE
			break
		fi
		count=$(($count+1))
		sleep 1
	done

	# Send command to Close ZigBee Coordinator network
	$PUBLISH -m '{"commands":[{"command":"plugin network-creator-security close-network","postDelayMs":100}]}' -t $ZIGBEE_COMMAND

	if [ $processExist -eq 0 ]
	then
		echo "Zigbee Network working Successfully" >> $TEST_LOG_FILE
		echo "$ZIGBEE_NETWORK_TEST PASS" > $TTY_NODE
	else
		echo "Zigbee Network is not working" >> $TEST_LOG_FILE
		echo "$ZIGBEE_NETWORK_TEST FAIL" > $TTY_NODE
	fi
}

factoryResetButton() {

	trap "rm -f $PIPE" EXIT

	if [[ ! -p $PIPE ]]; then
		mkfifo $PIPE
	fi
	tail -n 0 -f $LOG_FILE > $PIPE &

	echo "ready to read pipe" >> $TEST_LOG_FILE
	count=0
	while [ $factoryResetProcess -eq 1 ]
	do
		echo "In read mode === count = $count" >> $TEST_LOG_FILE
		while read -r -t 1 line; do
		if [ "$line" ]; then
			echo "$line" >> $TEST_LOG_FILE
			if echo "$line" | grep -q "Factory reset gateway"
			then
				echo "Factory reset button pressed" >> $TEST_LOG_FILE
				factoryResetProcess=0
				break
			fi
		fi
		done < "$PIPE"
		count=$(($count+1))
		if [ $count -eq $BUTTON_TIMEOUT ];then
			break
		fi
	done
	if [ $factoryResetProcess -eq 0 ]
	then
		echo "Factory reset test passed" >> $TEST_LOG_FILE
		echo "$FACTORY_RESET_BUTTON_TEST PASS" > $TTY_NODE
	else
		echo "factory reset test failed" >> $TEST_LOG_FILE
		echo "$FACTORY_RESET_BUTTON_TEST FAIL" > $TTY_NODE
	fi
}

hardwareResetButton() {

	trap "rm -f $PIPE" EXIT

	if [[ ! -p $PIPE ]]; then
		mkfifo $PIPE
	fi
	tail -n 0 -f $LOG_FILE > $PIPE &
	PID=$!
	echo "Process id === $PID"  >> $TEST_LOG_FILE

	echo "ready to read pipe" >> $TEST_LOG_FILE
	count=0
	while [ $HardwareReset -eq 1 ]
	do
		echo "In read mode === count = $count" >> $TEST_LOG_FILE
		while read -r -t 1 line; do
		if [ "$line" ]; then
			echo "$line" >> $TEST_LOG_FILE
			if echo "$line" | grep -q "stack ver."
			then
				echo "Hardaware reset button pressed" >> $TEST_LOG_FILE
				HardwareReset=0
				break
			fi
		fi
		done < "$PIPE"
		count=$(($count+1))
		if [ $count -eq $BUTTON_TIMEOUT ];then
			break
		fi
	done

	if [ $HardwareReset -eq 0 ]
	then
		echo "Hardware reset test passed" >> $TEST_LOG_FILE
		echo "$ZIGBEE_RESET_BUTTON_TEST PASS" > $TTY_NODE
	else
		echo "Hardware reset test failed" >> $TEST_LOG_FILE
		echo "$ZIGBEE_RESET_BUTTON_TEST FAIL" > $TTY_NODE
	fi
}

#check if node is present or not
if [ -e $TTY_NODE ]
then
	echo "" > $TEST_LOG_FILE
	#First Check if Zigbee Coordinator is flashed or not

#	while read -r line < $TTY_NODE; do
	# $line is the line read, do something with it
#	echo $line
	case $1 in

		MAC_ADDRESS)
			mac=`ifconfig eth0 | grep -o -E '([[:xdigit:]]{1,2}:){5}[[:xdigit:]]{1,2}'`
			echo "Device Mac address == $mac" >> $TEST_LOG_FILE
			echo "$DEVICE_MAC_ADDRESS=$mac" > $TTY_NODE
		;;
		ZIGBEE_FLASH_TEST)
			echo "ZIGBEE_FLASH_TEST started" >> $TEST_LOG_FILE
			zigbeeFlashTest
		;;

		ETHERNET_TEST)
			echo "ETHERNET_TEST started" >> $TEST_LOG_FILE
			ethernetTest $2
		;;
		ZIGBEE_NETWORK_TEST)
			echo "ZIGBEE_NETWORK_TEST started" >> $TEST_LOG_FILE
			zigbeeNetworkTest
		;;
		FACTORY_RESET_BUTTON_TEST)
			echo "FACTORY_RESET_BUTTON_TEST started" >> $TEST_LOG_FILE
			factoryResetButton
		;;
		ZIGBEE_RESET_BUTTON_TEST)
			echo "ZIGBEE_RESET_BUTTON_TEST started" >> $TEST_LOG_FILE
			hardwareResetButton
		;;
	esac
#	done
else
	echo "$TTY_NODE does't exist"
fi
