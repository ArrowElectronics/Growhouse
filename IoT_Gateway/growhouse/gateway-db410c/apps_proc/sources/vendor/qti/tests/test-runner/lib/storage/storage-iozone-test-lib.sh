#!/bin/sh
#Copyright (c) 2017, The Linux Foundation. All rights reserved.
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

# 
# This is a storage-iozone-test-lib sh file. This lib sh file has global functions
# definitions which will be helpful in storage iozone benchmark test scripts for different 
# storage devices like onboard_flash, sdcard-HC, sdcard-XC, sdcard-regular, usb.
# 

default_data_rate_kpi=0

# cleaning the temp folder and files after storage copyfile test. unmounting storage devices
cleanup()
{
  info_msg "cleaning"
  rm -r $read_write_folder_local > /dev/null 2>&1
  umount $read_write_folder_local > /dev/null 2>&1
  sleep 1
}

# run_storage_iozone_test <StorageDeviceType> <IozoneTestType> <WriteKPIThresholdKBps> <ReadKPIThresholdKBps>

run_storage_iozone_test()
{
  info_msg "Running Storage iozone Test"
  local storage_device_type=$1
  local iozone_test_type=$2
  local iozone_write_KPI_KBps_threshold=$3
  local iozone_read_KPI_KBps_threshold=$4

  local result=0

  info_msg "Storage Device Type         : $storage_device_type"
  info_msg "Iozone Test Type            : $iozone_test_type"
  info_msg "Iozone Read KPI Threshold   : $iozone_read_KPI_KBps_threshold KBps"
  info_msg "Iozone Write KPI Threshold  : $iozone_write_KPI_KBps_threshold KBps"


  if [ -z "$storage_device_type" ]
  then
    info_msg "ERROR: Storage device type argument is not specified. It should be one of the following: onboard_flash, sdcard-HC, sdcard-XC, sdcard-regular, usb"
    report_fail "storage_iozone_test_storage_device_type_validation"
    return 1
  fi

  if [ -z "$iozone_test_type" ]
  then
    info_msg "ERROR: iozone test type not supplied. Should be random or sequential"
    report_fail "storage_iozone_test_iozone_test_type_args_validation"
    return 1

  fi

  if [ "$iozone_test_type" != "random" ]
  then 
    if [ "$iozone_test_type" != "sequential" ]
    then
      info_msg "ERROR: iozone test type has wrong value. Should be random or sequential"
      report_fail "storage_iozone_test_iozone_test_type_validation"
      return 1
    fi
  fi

  if [ -z "$iozone_read_KPI_KBps_threshold" ]
  then 
    info_msg "Iozone Read Bench KPI not found, defaulting to $default_data_rate_kpi"
    iozone_read_KPI_KBps_threshold=$default_data_rate_kpi
  fi

  if [ -z "$iozone_write_KPI_KBps_threshold" ]
  then
    info_msg "Iozone Write Bench KPI not found, defaulting to $default_data_rate_kpi"
    iozone_write_KPI_KBps_threshold=$default_data_rate_kpi
  fi

  local read_write_folder_local='/home/linaro/read_write_folder_local'
  local filename=storage-iozone-test.bin

  # do some cleanup of potentially previously failed run
  cleanup

  mkdir -p $read_write_folder_local
  info_msg "Running test now"

  if [ "$storage_device_type" == "usb" ]
  then
    local storage_dev=$(lsblk | grep -o -E 'sd[a,z][1-9]')
    info_msg "USB device: $storage_dev"

  elif echo $storage_device_type | grep "sdcard.*"; then
    local storage_dev=$(lsblk | grep -o -E 'mmcblk1p[0-9]')
    info_msg "SDCARD device name is $storage_dev"
    local storage_device_filesystem=$(lsblk -f | grep -E 'mmcblk1p[0-9]' | cut -d" " -f3)
    if [ ! $storage_device_filesystem = "ext4" ]; then
      info_msg "Storage file system is=$storage_device_filesystem Test will run only with ext4 file system"
      info_msg "Please insert storage device with ext4 file system only"
      cleanup
      report_fail "storage_iozone_test_storage_file_system_check"
      return 1
    fi

    local sdcard_suffix=$(echo $storage_device_type | cut -d"-" -f2)
    #validating the SD card suffinx in argument
    if [ "$sdcard_suffix" != "HC" -a "$sdcard_suffix" != "XC" -a "$sdcard_suffix" != "regular" ]; then
      info_msg "ERROR: wrong storage_device_type SUFFIX argument given - $storage_device_type. It should be one of these: onboard_flash, sdcard-HC, sdcard-XC, sdcard-regular, usb"
      report_fail "storage_iozone_test_sd_card_type_check"
      return 1
    fi

  elif [ "$storage_device_type" != "onboard_flash" ]; then
    info_msg "ERROR: Wrong storage_device_type argument given - $storage_device_type. It should be one of the following: onboard_flash, sdcard-HC, sdcard-XC, sdcard-regular, usb"
    report_fail "storage_iozone_test_storage_storage_device_type_check"
    return 1
  fi

  if [ ! "$storage_device_type" == "onboard_flash" ]
  then
    info_msg "storage_device_type: $storage_device_type"

    if [ -z "$storage_device_type" ]
    then
      info_msg "ERROR: Mountable storage device not found. Please plugin $storage_device_type storage properly"
      report_fail "storage_iozone_test_storage_onboard_flash_device_check"
      return 1
    fi

    # mounting the storage device
    mount -o rw /dev/$storage_dev $read_write_folder_local
    exit_on_fail "mount_$storage_device_type"
    info_msg "$storage_device_type drive mounted to $read_write_folder_local"

  fi

  local logfile_iozone="${OUTPUT}/iozone.log"
  local iozone_cmd=""
  if [ "$iozone_test_type" == "random" ]; then
    iozone_cmd="iozone -azecI -+n -L64 -S32 -y4k -q1024k -n64m -g64m -i0 -i2 -o -b result.xls -f $read_write_folder_local/$filename > $logfile_iozone"
  elif [ "$iozone_test_type" == "sequential" ]; then
    iozone_cmd="iozone -azecI -+n -L64 -S32 -y512k -q512k -n1m -g1000m -i0 -i1 -o -b result.xls -f $read_write_folder_local/$filename > $logfile_iozone"
  fi

  info_msg "clearning cache"
  echo 3 > /proc/sys/vm/drop_caches
  sync

  info_msg "iozone command: $iozone_cmd"
  eval $iozone_cmd

  # post processing the results
  info_msg "analyzing data"
  if [ "$iozone_test_type" == "random" ]; then
    # look only on the smallest e.g. 4K size records
    local iozone_read_result="$( (grep "65536       4" $logfile_iozone | awk '{print $5}') )"
    local iozone_write_result="$( (grep "65536       4" $logfile_iozone | awk '{print $6}') )"
  elif [ "$iozone_test_type" == "sequential" ]; then
    # look on the largest records
    local iozone_read_result="$( (grep "524288" $logfile_iozone | grep '"524288"   0' -B 1 | sed -n '3p' | awk '{print $2}') 2>&1)"
    local iozone_write_result="$( (grep "524288" $logfile_iozone | grep '"524288"   0' -B 1 | sed -n '1p' | awk '{print $2}') 2>&1)"
  fi

  info_msg "The read criteria to pass is $iozone_read_KPI_KBps_threshold KBps and the result is $iozone_read_result KBps"
  info_msg "The write criteria to pass is $iozone_write_KPI_KBps_threshold KBps and the result is $iozone_write_result KBps"

  local write_res_msg="Write_KPI_=_$iozone_write_KPI_KBps_threshold-Kbps.__Measured_iozone-write-data-rate-$iozone_write_result-Kbps"

  if [ "$iozone_write_result" -lt $iozone_write_KPI_KBps_threshold ]; then
    info_msg "KPI ERROR: Low write date rate, required: $iozone_write_KPI_KBps_threshold KBps, measured: $iozone_write_result KBps"
    report_fail "$write_res_msg"
    result=1
  else
    report_pass "$write_res_msg"
  fi

 local read_res_msg="Read_KPI_=_$iozone_read_KPI_KBps_threshold-Kbps.__measured_read_iozone_data_rate-$iozone_read_result-Kbps"

  if [ "$iozone_read_result" -lt $iozone_read_KPI_KBps_threshold ]; then
    info_msg "KPI ERROR: Low read date rate, required: $iozone_read_KPI_KBps_threshold KBps, measured: $iozone_read_result KBps"
    report_fail "$read_res_msg"
    result=1
  else
    report_pass "$read_res_msg"
  fi

  cleanup

  if [ "$result" -eq "0" ] ; then
    return 0
  else
    return 1
  fi
}

