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
# runs iozone - IOzone is a filesystem benchmark tool. The benchmark generates and measures a variety of file operations.



echo 
echo "=====> Running Test (iozone) <======"
echo 
date

MODE=$1
RANDOM_OR_SEQUENTIAL=$2
READ_KPI=$3
WRITE_KPI=$4

echo mode is $MODE
echo random or sequential is $RANDOM_OR_SEQUENTIAL
echo read pass threshold is $READ_KPI MBps
echo write pass threshold is $WRITE_KPI MBps


if [ -z "$MODE" ]
then
  echo ERROR: Mode argument is not specified. It should be one of the following: usb, sdcard or onboard_flash
  date
  exit 7
fi

if [ -z "$RANDOM_OR_SEQUENTIAL" ]
then
  echo ERROR: Random-or-Sequential parameter not supplied. Should be random or sequential
  date
  exit 6
fi
if [ "$RANDOM_OR_SEQUENTIAL" != "random" ]
then 
  if [ "$RANDOM_OR_SEQUENTIAL" != "sequential" ]
  then
    echo ERROR: Random-or-Sequential parameter has wrong value. Should be random or sequential
    date
    exit 5
  fi
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
  mount -o rw /dev/$DEVICE_NAME $READ_WRITE_FOLDER
  echo USB drive mounted to $READ_WRITE_FOLDER
  mount | grep read_write

fi

echo READ_WRITE_FOLDER is $READ_WRITE_FOLDER 

COMMAND_TO_EXECUTE=""
if [ "$RANDOM_OR_SEQUENTIAL" == "random" ]; then
  COMMAND_TO_EXECUTE="iozone -azecI -+n -L64 -S32 -y4k -q1024k -n64m -g64m -i0 -i2 -o -b result.xls -f $READ_WRITE_FOLDER/$FILENAME > result.txt"
elif [ "$RANDOM_OR_SEQUENTIAL" == "sequential" ]; then
  COMMAND_TO_EXECUTE="iozone -azecI -+n -L64 -S32 -y512k -q512k -n1m -g1000m -i0 -i1 -o -b result.xls -f $READ_WRITE_FOLDER/$FILENAME > result.txt"
fi


echo
echo clearning cache...
echo 3 > /proc/sys/vm/drop_caches
echo
sync

echo COMMAND_TO_EXECUTE is $COMMAND_TO_EXECUTE
eval $COMMAND_TO_EXECUTE


# post processing the results
echo  
echo analyzing data...
echo
if [ "$RANDOM_OR_SEQUENTIAL" == "random" ]; then
  # look only on the smallest e.g. 4K size records
  READ_RES="$( (grep "65536       4" result.txt | awk '{print $5}') )"
  WRITE_RES="$( (grep "65536       4" result.txt | awk '{print $6}') )"
elif [ "$RANDOM_OR_SEQUENTIAL" == "sequential" ]; then
  # look on the largest records
  READ_RES="$( (grep "524288" result.txt | grep '"524288"   0' -B 1 | sed -n '3p' | awk '{print $2}') 2>&1)"
  WRITE_RES="$( (grep "524288" result.txt | grep '"524288"   0' -B 1 | sed -n '1p' | awk '{print $2}') 2>&1)"
fi

echo "The read criteria to pass is $READ_KPI KBps and the result is $READ_RES KBps"
echo "The write criteria to pass is $WRITE_KPI KBps and the result is $WRITE_RES KBps"


if [ "$WRITE_RES" -lt $WRITE_KPI ]; then 
	RESULT=1 
elif [ "$READ_RES" -lt $READ_KPI ]; then 
	RESULT=2
else
  RESULT=0
fi

#writing result to file, to be read by test-executor.sh and used in log files
echo "read = $READ_RES. write = $WRITE_RES" > test_string_ret_val.txt

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
echo "=====> Finished Test (iozone) <====="
echo 
date
exit $RESULT

