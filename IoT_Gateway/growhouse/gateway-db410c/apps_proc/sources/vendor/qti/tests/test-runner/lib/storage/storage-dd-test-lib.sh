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
# This is a storage-dd-lib sh file. This lib sh file has a global function
# definitions which will be helpful in storage dd test scripts for different 
# storage devices like onboard_flash, sdcard-HC, sdcard-XC, sdcard-regular, usb.
# 
#
default_data_rate_kpi=0
default_file_size=1024

# cleaning the temp folder and files after storage copyfile test. unmounting storage devices

cleanup()
{
  info_msg "cleaning"
  umount $read_write_folder > /dev/null 2>&1
  rm -r $read_write_folder > /dev/null 2>&1
  sleep 1
}

#run_storage_dd_test <StorageDeviceType> <TestFileSizeinMB> <WriteKPIThresholdMBps> <ReadKPIThresholdMBps>

run_storage_dd_test()
{
  info_msg "Running Storage dd Test"

  local storage_device_type=$1
  local dd_test_filesize=$2
  local dd_write_KPI_MBps_threshold=$3
  local dd_read_KPI_MBps_threshold=$4

  result=0

  info_msg "Storage Device Type     : $storage_device_type"
  info_msg "dd Test File Size (MB)  : $dd_test_filesize"
  info_msg "dd Read KPI Threshold   : $dd_read_KPI_MBps_threshold MBps"
  info_msg "dd Write KPI Threshold  : $dd_write_KPI_MBps_threshold MBps"

  if [ -z "$storage_device_type" ]
  then
    info_msg "ERROR: Storage device type is not specified. It should be one of the following: onboard_flash, sdcard-HC, sdcard-XC, sdcard-regular, usb"
    report_fail "run_storage_dd_test_storage_device_type_validation"
    return 1

  fi

  if [ -z "$dd_test_filesize" ]
  then
    info_msg "File size in MB, not supplied. Defaulting to $default_file_size MB"
    dd_test_filesize=$default_file_size
  fi

  if [ -z "$dd_read_KPI_MBps_threshold" ]
  then 
    info_msg "dd Read Bench KPI not found, defaulting to $default_data_rate_kpi"
    dd_read_KPI_MBps_threshold=$default_data_rate_kpi
  fi

  if [ -z "$dd_write_KPI_MBps_threshold" ]
  then
    info_msg "dd Write Bench KPI not found, defaulting to $default_data_rate_kpi"
    dd_write_KPI_MBps_threshold=$default_data_rate_kpi
  fi

  local read_write_folder='/home/linaro/read_write_folder'
  local filename=storage-dd-test.bin

  # do some cleanup of potenitally previously failed run
  cleanup

  mkdir -p $read_write_folder
  info_msg "Running test now"

  if [ "$storage_device_type" = "usb" ]
  then
    local storage_dev=$(lsblk | grep -o -E 'sd[a,z][1-9]')
    info_msg "USB device: $storage_dev"

  elif echo $storage_device_type | grep "sdcard.*"; then
    local storage_dev=$(lsblk | grep -o -E 'mmcblk1p[0-9]') > /dev/null 2>&1
    info_msg "SDCARD device: $storage_dev"
    local device_file_system=$(lsblk -f | grep -E 'mmcblk1p[0-9]' | cut -d" " -f3)
    if [ ! $device_file_system = "ext4" ]; then
      info_msg "Storage file system is=$device_file_system Test will run only with ext4 file system"
      info_msg "Please insert storage device with ext4 file system only"
      cleanup
      report_fail "run_storage_dd_test_storage_file_system_check"
      return 1
    fi

    local sdcard_suffix=$(echo $storage_device_type | cut -d"-" -f2)
    #validating the SD card suffinx in argument
    if [ "$sdcard_suffix" != "HC" -a "$sdcard_suffix" != "XC" -a "$sdcard_suffix" != "regular" ]; then
      info_msg "ERROR: wrong storage_device_type SUFFIX argument given - $storage_device_type. It should be one of these: onboard_flash, sdcard-HC, sdcard-XC, sdcard-regular, usb"
      report_fail "storage_dd_test_sd_card_type_check"
      return 1
    fi

  elif [ "$storage_device_type" != "onboard_flash" ]; then
    info_msg "ERROR: Wrong storage_device_type argument given - $storage_device_type. It should be one of the following: onboard_flash, sdcard-HC, sdcard-XC, sdcard-regular, usb"
    report_fail "storage_dd_test_storage_storage_device_type_check"
    return 1
  fi

# cheking if mount the device or not. if it's not onboard flash it will mount
  if [ ! "$storage_device_type" == "onboard_flash" ]; then
    info_msg "storage_dev: $storage_dev"

    if [ -z "$storage_dev" ]; then
      info_msg "ERROR: Mountable storage device not found. Please plugin $storage_device_type storage properly" 
      report_fail "run_storage_dd_test_storage_onboard_flash_device_check"
      return 1
    fi

    # mounting the storage device
    mount -o rw /dev/$storage_dev $read_write_folder >/dev/null 2>&1
    exit_on_fail "mount_$storage_dev"
    info_msg "$storage_device_type drive mounted to $read_write_folder"

  fi

  info_msg "Clearning cache"
  echo 3 > /proc/sys/vm/drop_caches
  sync

  local dd_cmd_write="dd if=/dev/urandom of=$read_write_folder/$filename bs=1M count=$dd_test_filesize conv=fsync"
  info_msg "dd Write Command: $dd_cmd_write"

  info_msg "Performing dd write test"
  local dd_logfile_write="${OUTPUT}/dd_write_test.log"
  echo "$dd_cmd_write" > ${OUTPUT}/to_exec.sh
  echo "sync" >> ${OUTPUT}/to_exec.sh
  chmod 777 ${OUTPUT}/to_exec.sh
  (time sh ${OUTPUT}/to_exec.sh) &> "$dd_logfile_write"
  info_msg "dd write test done"

  info_msg "Clearning cache"
  echo 3 > /proc/sys/vm/drop_caches
  sync

  local dd_cmd_read="dd if=$read_write_folder/$filename of=/dev/null bs=1M"
  info_msg "Read Command: $dd_cmd_read"

  info_msg "Performing dd read test"
  local dd_logfile_read="${OUTPUT}/read_test.log"
  echo "$dd_cmd_read" > ${OUTPUT}/to_exec.sh
  echo "sync" >> ${OUTPUT}/to_exec.sh
  chmod 777 ${OUTPUT}/to_exec.sh
  (time sh ${OUTPUT}/to_exec.sh) &> "$dd_logfile_read"
  info_msg "dd read test done"

  # post processing the results
  info_msg "Analyzing data"
  local filesize=$( (ls -l $read_write_folder/$filename | awk '{ print $5 }' ) )
  info_msg "File size: $filesize bytes"

  local dd_write_time=$( (cat $dd_logfile_write | grep real | awk -F'[/ms\t]' '{print $2 * 60 + $3 }' ) )
  info_msg "Time took to write file: $dd_write_time seconds"

  local dd_read_time=$( (cat $dd_logfile_read | grep real | awk -F'[/ms\t]' '{print $2 * 60 + $3 }' ) )
  info_msg "Time took to read file: $dd_read_time seconds"

  local dd_write_speed=$( (echo $filesize $dd_write_time | awk '{ print $1 / $2 / 1024 / 1024 }' ) )
  local dd_write_speed_INT=$( (awk "BEGIN {printf \"%d\n\",$dd_write_speed}") )
  info_msg "dd write date rate: $dd_write_speed MBps"

  local dd_read_speed=$( (echo $filesize $dd_read_time | awk '{ print $1 / $2 / 1024 / 1024 }' ) )
  local dd_read_speed_INT=$( (awk "BEGIN {printf \"%d\n\",$dd_read_speed}") )
  info_msg "dd read data rate: $dd_read_speed MBps"


  local write_res_msg="Write_KPI_=_$dd_write_KPI_MBps_threshold-Mbps.__measured_write_data_rate:$dd_write_speed-Mbps"

  if [ "$dd_write_speed_INT" -lt "$dd_write_KPI_MBps_threshold" ]
  then
    info_msg "KPI FAIL: Low write date rate! required: $dd_write_KPI_MBps_threshold MBps. measured rate: $dd_write_speed MBps."
    report_fail "$write_res_msg"
    result=1
  else
    report_pass "$write_res_msg"
  fi


  local read_res_msg="Read_KPI_=_$dd_read_KPI_MBps_threshold-Mbp.__measured_read_data-rate-$dd_read_speed-Mbps"

  if [ "$dd_read_speed_INT" -lt "$dd_read_KPI_MBps_threshold" ]
  then
    info_msg "KPI FAIL: Low read date rate! required: $dd_read_KPI_MBps_threshold MBps, measured: $dd_read_speed MBps,"
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

