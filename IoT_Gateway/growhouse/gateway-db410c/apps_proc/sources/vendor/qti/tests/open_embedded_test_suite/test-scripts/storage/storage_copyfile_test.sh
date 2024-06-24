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

# 
# this test copies the file from folder1 to folder2 and then from folder2 to folder1
# and checks for file validity and desired copy speeds (KPIs)
# 


echo 
echo "=====> Running Test (copy file) <======"
echo 
date

MODE1=$1
MODE2=$2
FILE_SIZE_IN_MB=$3
COPY1_KPI=$4
COPY2_KPI=$5
RESULT=0

echo mode1 is $MODE1
echo mode2 is $MODE2
echo file size is $FILE_SIZE_IN_MB
echo copy rate 1 pass threshold is $COPY1_KPI MBps
echo copy rate 2 pass threshold is $COPY2_KPI MBps


if [ -z "$MODE1" ]
then
  echo ERROR: folder1 is not specified. It should be one of the following: usb, sdcard or onboard_flash
  date
  exit 7
fi

if [ -z "$MODE2" ]
then
  echo ERROR: folder2 is not specified. It should be one of the following: usb, sdcard or onboard_flash
  date
  exit 7
fi

if [ -z "$FILE_SIZE_IN_MB" ]
then
  echo File size in MB, not supplied. Defaulting to 1024MB
  FILE_SIZE_IN_MB=1024
fi

if [ -z "$COPY1_KPI" ]
then 
  echo Copy1 KPI not given, defaulting to 0
  COPY1_KPI=0
fi

if [ -z "$COPY2_KPI" ]
then
  echo Copy2 KPI not given, defaulting to 0
  COPY2_KPI=0
fi

FOLDER1='/home/linaro/folder1'
FOLDER2='/home/linaro/folder2'
FILENAME_SRC=test.bin
FILENAME_DEST=test.2.bin
FILENAME_ROUNDTRIP=test.3.bin


# do some ome cleanup of potenitally previously failed run
echo pre-test cleaning...
umount $FOLDER1 &> /dev/null
rm -rf $FOLDER1 &> /dev/null
umount $FOLDER2 &> /dev/null
rm -rf $FOLDER2 &> /dev/null
sleep 1
mkdir $FOLDER1
mkdir $FOLDER2
echo cleaning done...
echo  

#
# gets mode argument and decides if need to mount according to this mode
#
mount_if_needed () {
  MODE=$1
  FOLDER_TO_MOUNT=$2
  echo mount_if_needed $MODE, $FOLDER_TO_MOUNT called...

  unset $DEVICE_NAME
  
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
	  mount -o rw /dev/$DEVICE_NAME $FOLDER_TO_MOUNT
	  echo device mounted to $FOLDER_TO_MOUNT
	  mount | grep $FOLDER_TO_MOUNT

	fi
}

echo "-------------- preparing FOLDER1 -------------------"
mount_if_needed $MODE1 $FOLDER1
echo "---------------------- done ------------------------"
echo 
echo "-------------- preparing FOLDER2 -------------------"
mount_if_needed $MODE2 $FOLDER2
echo "---------------------- done ------------------------"
echo

echo FOLDER1 is $FOLDER1
echo FOLDER2 is $FOLDER2

# create the temp file $FOLDER1/$FILENAME_SRC
echo
echo creating the temp $FOLDER1/$FILENAME_SRC
dd if=/dev/urandom of=$FOLDER1/$FILENAME_SRC bs=1M count=$FILE_SIZE_IN_MB conv=fsync
sync

echo
echo clearning cache...
echo 3 > /proc/sys/vm/drop_caches
echo
sync

copy_file () {
  SRC=$1
  DEST=$2
  LOGFILE=$3
  
  echo  
  COMMAND_COPY="cp $SRC $DEST"
  echo COMMAND_COPY is $COMMAND_COPY
  
  echo
  echo clearning cache...
  echo 3 > /proc/sys/vm/drop_caches
  echo
  sync
  
  echo  
  echo performing copy from $SRC to $DEST
  echo "$COMMAND_COPY" > to_exec.sh
  echo "sync" >> to_exec.sh
  chmod 777 to_exec.sh
  ( time ./to_exec.sh ) &> $LOGFILE
  echo copy done
  echo "======= cat to_exec.sh ========"	
  cat to_exec.sh
  echo "======= cat LOGFILE ========"
  cat $LOGFILE
  
  echo checking files...
  if cmp -s $SRC $DEST ; then
    echo files are identical - ok
  else
    echo ERROR: files are NOT identical!
	RESULT=10
  fi
  
}

copy_file $FOLDER1/$FILENAME_SRC $FOLDER2/$FILENAME_DEST copy1.log

FILE_SIZE=$( (ls -l $FOLDER1/$FILENAME_SRC | awk '{ print $5 }' ) )
rm $FOLDER1/$FILENAME_SRC

copy_file $FOLDER2/$FILENAME_DEST $FOLDER1/$FILENAME_ROUNDTRIP copy2.log

rm $FOLDER2/$FILENAME_DEST
rm $FOLDER1/$FILENAME_ROUNDTRIP

# post processing the results
echo  
echo analyzing data...
echo

echo file size is $FILE_SIZE bytes

TIME_COPY1=$( (cat copy1.log | grep real | awk -F'[/ms\t]' '{print $2 * 60 + $3 }' ) )
echo time took to copy file from $MODE1 to $MODE2: $TIME_COPY1 seconds
TIME_COPY2=$( (cat copy2.log | grep real | awk -F'[/ms\t]' '{print $2 * 60 + $3 }' ) )
echo time took to copy file from $MODE2 to $MODE1: $TIME_COPY2 seconds

COPY_1_SPEED=$( (echo $FILE_SIZE $TIME_COPY1 | awk '{ print $1 / $2 / 1024 / 1024 }' ) )
COPY_1_SPEED_INT=$( (awk "BEGIN {printf \"%d\n\",$COPY_1_SPEED}") )
echo Copy from $MODE1 to $MODE2 data rate: $COPY_1_SPEED MBps

COPY_2_SPEED=$( (echo $FILE_SIZE $TIME_COPY2 | awk '{ print $1 / $2 / 1024 / 1024 }' ) )
COPY_2_SPEED_INT=$( (awk "BEGIN {printf \"%d\n\",$COPY_2_SPEED}") )
echo Copy from $MODE2 to $MODE1 data rate: $COPY_2_SPEED MBps

echo "Copy from $MODE1 to $MODE2 data rate: $COPY_1_SPEED MBps, Copy from $MODE2 to $MODE1 data rate: $COPY_2_SPEED MBps" > test_string_ret_val.txt

if [ "$RESULT" -eq "0" ] ; then
  if [ "$COPY_1_SPEED_INT" -lt "$COPY1_KPI" ]; then 
	RESULT=1 
  elif [ "$COPY_2_SPEED_INT" -lt "$COPY2_KPI" ]; then 
	RESULT=2
  else
    RESULT=0
  fi
fi


# do some cleanup after test
echo  
echo cleaning after test...
umount $FOLDER1 &> /dev/null
rm -rf $FOLDER1 &> /dev/null
umount $FOLDER2 &> /dev/null
rm -rf $FOLDER2 &> /dev/null
sleep 1
mkdir $FOLDER1
mkdir $FOLDER2
echo cleaning done...
echo  

echo  
if [ "$RESULT" -eq "0" ]
then
  echo " + + P A S S + + "
else
  echo " - - F A I L - - "
fi
echo
echo "=====> Finished Test (copy file) <====="
echo 
date
exit $RESULT

