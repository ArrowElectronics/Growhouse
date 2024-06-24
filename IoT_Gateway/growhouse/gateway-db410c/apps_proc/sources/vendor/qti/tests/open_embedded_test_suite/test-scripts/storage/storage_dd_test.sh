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
# script tests resd/write data rate capabilities of peripheral devices (USB flash memory, internal flash memeory, SD card)


echo 
echo "=====> Running Test (DD read and write test) <======"
echo 
date

MODE=$1
FILE_SIZE_IN_MB=$2
READ_KPI=$3
WRITE_KPI=$4

PASSORFAIL=2  
echo mode is $MODE
echo file size in MB is $FILE_SIZE_IN_MB
echo read pass threshold is $READ_KPI MBps
echo write pass threshold is $WRITE_KPI MBps

if [ -z "$MODE" ]
then
  echo ERROR: Mode argument is not specified. It should be one of the following: usb, sdcard or onboard_flash
  date
  exit 5
fi

if [ -z "$FILE_SIZE_IN_MB" ]
then
  echo File size in MB, not supplied. Defaulting to 1024MB
  FILE_SIZE_IN_MB=1024
fi

if [ -z "$READ_KPI" ]
then 
  echo Read KPI not given, defaulting to 0
  READ_KPI=0
fi

if [ -z "$WRITE_KPI" ]
then
  echo Write KPI not given, defaulting to 0
  WRITE_KPI=0
fi

READ_WRITE_FOLDER='/home/linaro/read_write_folder'
FILENAME=test.bin

# do some ome cleanup of potenitally previously failed run
echo pre-test cleaning...
umount $READ_WRITE_FOLDER &> /dev/null
rm -rf $READ_WRITE_FOLDER &> /dev/null
sleep 1
mkdir $READ_WRITE_FOLDER
echo cleaning done...
echo  
echo running test now...

if [ "$MODE" == "usb" ]
then
  
  echo "mode: usb"
  echo  
 
  DEVICE_NAME=$(lsblk | grep -o -E 'sd[a,z][1-9]')
  echo USB device name is $DEVICE_NAME

elif [ "$MODE" == "sdcard" ]
then

  echo "mode: sdcard"
  echo  

  DEVICE_NAME=$(lsblk | grep -o -E 'mmcblk1p[0-9]')
  echo SDCARD device name is $DEVICE_NAME

elif [ "$MODE" == "onboard_flash" ]
then

  echo "mode: onboard_flash"
  echo  

else
  echo  
  echo "ERROR: wrong mode argument given - $MODE. It should be one of the following: usb, sdcard or onboard_flash"
  echo 
  date
  exit 4
fi

if [ ! "$MODE" == "onboard_flash" ]
then

  echo "DEVICE_NAME is $DEVICE_NAME"

  if [ -z "$DEVICE_NAME" ]
  then
    echo  
    echo "ERROR: Mountable storage device not found. Please plugin $MODE storage properly" 
    echo  
    date
    exit 2
  fi

  # mount
 # mount -t vfat -o rw /dev/$DEVICE_NAME $READ_WRITE_FOLDER
mount -o rw /dev/$DEVICE_NAME $READ_WRITE_FOLDER
  echo USB drive mounted to $READ_WRITE_FOLDER

fi

echo
echo clearning cache
echo 3 > /proc/sys/vm/drop_caches
sync

COMMAND_WRITE="dd if=/dev/urandom of=$READ_WRITE_FOLDER/$FILENAME bs=1M count=$FILE_SIZE_IN_MB conv=fsync"

echo  
echo COMMAND_WRITE is $COMMAND_WRITE

echo  
echo performing write test
echo "$COMMAND_WRITE" > to_exec.sh
echo "sync" >> to_exec.sh
chmod 777 to_exec.sh
( time ./to_exec.sh ) &> write_time.log
echo write test done

echo
echo clearning cache
echo 3 > /proc/sys/vm/drop_caches
sync

COMMAND_READ="dd if=$READ_WRITE_FOLDER/$FILENAME of=/dev/null bs=1M"
echo COMAND_READ is $COMMAND_READ

echo  
echo performing read test
echo "$COMMAND_READ" > to_exec.sh
echo "sync" >> to_exec.sh
chmod 777 to_exec.sh
( time ./to_exec.sh ) &> read_time.log
echo read test done


# post processing the results
echo  
echo analyzing data...
FILE_SIZE=$( (ls -l $READ_WRITE_FOLDER/$FILENAME | awk '{ print $5 }' ) )
echo file size is $FILE_SIZE bytes

TIME_WRITE=$( (cat write_time.log | grep real | awk -F'[/ms\t]' '{print $2 * 60 + $3 }' ) )
echo time took to write file: $TIME_WRITE seconds

TIME_READ=$( (cat read_time.log | grep real | awk -F'[/ms\t]' '{print $2 * 60 + $3 }' ) )
echo time took to read file: $TIME_READ seconds

WRITE_SPEED=$( (echo $FILE_SIZE $TIME_WRITE | awk '{ print $1 / $2 / 1024 / 1024 }' ) )
WRITE_SPEED_INT=$( (awk "BEGIN {printf \"%d\n\",$WRITE_SPEED}") )
echo Write date rate: $WRITE_SPEED MBps

READ_SPEED=$( (echo $FILE_SIZE $TIME_READ | awk '{ print $1 / $2 / 1024 / 1024 }' ) )
READ_SPEED_INT=$( (awk "BEGIN {printf \"%d\n\",$READ_SPEED}") )
echo Read data rate: $READ_SPEED MBps

echo "read data rate: $READ_SPEED MBps, write data rate: $WRITE_SPEED MBps" > test_string_ret_val.txt

if [ "$WRITE_SPEED_INT" -lt "$WRITE_KPI" ]
then
  echo "ERROR: Low write date rate, actual: $WRITE_SPEED MBps, required: $WRITE_KPI MBps"
  RESULT=2
fi

RESULT=0
if [ "$READ_SPEED_INT" -lt "$READ_KPI" ]
then
  echo "ERROR: Low read date rate, actual: $READ_SPEED MBps, required: $READ_KPI MBps"
  RESULT=1
fi

# do some cleanup after test
echo  
echo cleaning after test...
umount $READ_WRITE_FOLDER &> /dev/null
rm -rf $READ_WRITE_FOLDER &> /dev/null
sleep 1
mkdir $READ_WRITE_FOLDER
echo cleaning done...

echo  
if [ "$RESULT" -eq "0" ]
then
  echo " + + P A S S + + "
else
  echo " - - F A I L - - "
fi
echo
echo "=====> Finished Test (DD read and write test) <====="
echo 
date
exit $RESULT

