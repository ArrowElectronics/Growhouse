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
# This is a storage-copfile-lib sh file. This lib sh file has global function
# definations which will be helpful in storage copyfile test scripts for different 
# storage devices like onboard_flash, sdcard-HC, sdcard-XC, sdcard-regular, usb.
# 

pt_mode_file="../../../lib/storage/pt_mode_is_on"
default_file_size=1024
default_file_size_pt=1
default_data_rate_kpi=0
default_if_measure_throughput=true
default_if_measure_throughput_pt=false
default_connected_devices_num=1

### do no change this value! source is always flash memmory and currently there is only one.
default_source_storage_device_count=1

# cleaning the temp folder and files after storage copyfile test. unmounting storage devices

cleanup()
{
  info_msg "cleaning"

  info_msg "unmounting dir $folder_src_to_dest"
  umount $folder_src_to_dest
  info_msg "unmounting dir $folder_dest_to_src"
  umount $folder_dest_to_src

  info_msg "removing dir $folder_src_to_dest "
  rm -r $folder_src_to_dest
  info_msg "removing dir $folder_dest_to_src"
  rm -r $folder_dest_to_src
}

# checking which of mmc devices are SD card

get_mmc_num()
{
  local mmc_dir="/sys/class/mmc_host/"
  local i=0
  local limit=9

  while [ $i -le $limit ]
  do
    if [ -d "$mmc_dir"mmc"$i" -a -d "$mmc_dir"mmc"$i"/mmc"$i"* ]; then
      local mmc="$mmc_dir"mmc"$i"/mmc"$i"*
      local mmc_type=$( (cat $mmc/uevent | grep MMC_TYPE | awk -F= '{print $2 }' ) )
      if [ "$mmc_type" = "SD" ]; then
	MMC_NUM="$MMC_NUM $i"
      fi
    fi
    i=$((i+1))
  done  
}

# preparing list of all plugged in sd card devices

prepare_sd_card_devices_list() {
  local source_or_destination=$1
  local device_list=""
  MMC_NUM=""
  get_mmc_num
  
  if [ ! "$MMC_NUM" = "" ]; then
    for i in $MMC_NUM
    do
      local new_device=$(lsblk | grep -o -E mmcblk"$i"p[0-9])
      device_list="$device_list $new_device"
    done
    if [ "$source_or_destination" = "SOURCE" ]; then
      LIST_OF_SOURCE_STORAGE_DEVICES="$device_list"
    else
      LIST_OF_DESTINATION_STORAGE_DEVICES="$device_list"
    fi
  else
    return 1
  fi
}

# checking if all SD cards have ext4 filesystem
# arg1 - SOURCE/DESTINATION
check_sd_card_filesystem_type() {
  local source_or_destination=$1
  local device_list=""
  local sd_card_file_system=""
  
  if [ "$source_or_destination" = "SOURCE" ]; then
    device_list="$LIST_OF_SOURCE_STORAGE_DEVICES"
  else
    device_list="$LIST_OF_DESTINATION_STORAGE_DEVICES"
  fi

  for i in $device_list
  do
    sd_card_file_system=$(lsblk -f | grep -E $i | cut -d" " -f3)
    if [ "$sd_card_file_system" = "" ]; then
      sd_card_file_system=$(lsblk -f | grep -E $i | cut -d" " -f2)
    fi
    if [ ! "$sd_card_file_system" = "ext4" ]; then
      info_msg "storage file system is=$sd_card_file_system. test will run only with ext4 file system"
      info_msg "Please insert storage device with ext4 file system only"
      info_msg "In order to format your SD card in ext4 filesystem, perform the following actions:"
      info_msg "  1. Find your SD card in /dev directory"
      info_msg "  2. You should have 2 devices: somename and somenamepX (f.e.: mmcblk1 and mmcblk1p1)"
      info_msg "  3. mkfs.ext4 /dev/somenamepX (f.e: mkfs.ext4 /dev/mmcblk1p1)"
      return 1
    fi
  done
}

# validating the SD card type suffix

validate_sd_card_type() {
  local storage_device_type=$1
  local sdcard_suffix=$(echo $storage_device_type | cut -d"-" -f2)

  if [ "$sdcard_suffix" != "HC" -a "$sdcard_suffix" != "XC" -a "$sdcard_suffix" != "regular" ]; then
    info_msg "ERROR: wrong storage_device_type SUFFIX argument given - $storage_device_type. It should be one of these: onboard_flash, sdcard-HC, sdcard-XC, sdcard-regular, usb"
    return 1
  fi
}

# checking which of sd devices are USB

get_usb_letter() {
  local sd_dir="/sys/block/"
  local sd_prefix="${sd_dir}sd"
  local if_usb=0

  local if_any_sd=$(ls $sd_dir | grep sd)
  if [ ! "$if_any_sd" = "" ]; then
    for i in "$sd_prefix"*
    do
      if_usb=$(cat "$i"/removable)
      if [ "$if_usb" = "1" ]; then
        USB_LET="$USB_LET ${i#"$sd_prefix"}"
      fi
    done
  fi
}

# preparing list of all plugged in usb devices

prepare_usb_devices_list() {
  local source_or_destination=$1
  local device_list=""
  USB_LET=""
  get_usb_letter
  if [ ! "$USB_LET" = "" ]; then
    for i in $USB_LET
    do
      local new_device=$(lsblk | grep -o -E sd"$i"[1-9])
      device_list="$device_list $new_device"
    done
    if [ "$source_or_destination" = "SOURCE" ]; then
      LIST_OF_SOURCE_STORAGE_DEVICES="$device_list"
    else
      LIST_OF_DESTINATION_STORAGE_DEVICES="$device_list"
    fi
  else
    return 1
  fi
}

# counting devices found

count_devices_found() {
  local source_or_destination=$1
  unset device_list
  if [ "$source_or_destination" = "SOURCE" ]; then
    device_list="$LIST_OF_SOURCE_STORAGE_DEVICES"
  else
    device_list="$LIST_OF_DESTINATION_STORAGE_DEVICES"
  fi
  local count=0
  
  for i in $device_list
  do
    count=$((count+1))
  done
  
  return $count
}

# copy_file <Source> <Destination> <Logfile>

copy_file () {
  local src=$1
  local dest=$2
  local logfile=$3
  local if_check_time=$4

  if [ $# -lt 3 ];then
    info_msg "copy_file function requires 3 arguments. usage: copy_file <Source> <Destination> <Logfile>"
    return 1
  fi

  if [ ! -f $src ];then
    info_msg "$src is not a file. Please give valid file in copy_file <Source> argument. usage: copy_file <Source> <Destination> <Logfile>"
    return 1
  fi

  command_copy="cp $src $dest"
  info_msg "Copy command: $command_copy"

  info_msg "clearning cache"
  echo 3 > /proc/sys/vm/drop_caches
  sync

  info_msg "performing copy from $src to $dest"
  echo "$command_copy" > ${OUTPUT}/to_exec.sh
  echo "sync" >> ${OUTPUT}/to_exec.sh
  chmod 777 ${OUTPUT}/to_exec.sh

  if [ "$if_check_time" = "true" ]; then
    time -p sh ${OUTPUT}/to_exec.sh > $logfile 2>&1
    info_msg "copy done"
  else
    ${OUTPUT}/to_exec.sh
  fi

  info_msg "checking files"
  if cmp -s $src $dest ; then
    info_msg "files are identical "
    return 0
  else
    info_msg "ERROR: files are NOT identical!"
    return 1
  fi

}

# checking if storage device type is a known one 
if_known_storage_device_type() {
  local storage_device_type=$1
  
  if [ "$storage_device_type" = "onboard_flash" -o "$storage_device_type" = "usb" -o "$storage_device_type" = "sdcard-HC" -o "$storage_device_type" = "sdcard-XC" -o "$storage_device_type" = "sdcard-regular" ]; then
    true
  else
    false
  fi
}


# checking if input received allowes to perform test
analyze_input_params() {
  local source_storage_device_type=$1
  local destination_storage_device_type=$2
  local if_ligitimate_device_type=false
  
  info_msg "Analyzing input data"
  
  info_msg "Source Storage Device Type                : $source_storage_device_type"
  info_msg "Destination Storage Device Type           : $destination_storage_device_type"
  info_msg "Number of devices connected               : $CONNECTED_DEVICES_NUM"
  info_msg "Copy Test File size                       : $TEST_FILESIZE_IN_MB MB"
  info_msg "Source to Destination KPI Threshold MBps  : $SOURCE_TO_DESTINATION_KPI_THRESHOLD_MBPS MBps"
  info_msg "Destination to Source KPI Threshold MBps  : $DESTINATION_TO_SOURCE_KPI_THRESHOLD_MBPS MBps"
  info_msg "If to measure file copy throughput        : $IF_MEASURE_THROUGHPUT"
  
  if [ -z "$source_storage_device_type" ]
  then
    info_msg "ERROR:_Source storage device is not specified. It should be one of the following:"
    info_msg "onboard_flash, sdcard-HC, sdcard-XC, sdcard-regular, usb"
    report_fail "storage_copyfile_test_storage_device_type_validation"
    return 1;
  fi
  
  if_ligitimate_device_type=$(if_known_storage_device_type $source_storage_device_type)
  if [ "$if_ligitimate_device_type" = false ]
  then
    info_msg "ERROR: Source storage device is unknown. It should be one of the following:"
    info_msg "onboard_flash, sdcard-HC, sdcard-XC, sdcard-regular, usb"
    report_fail "storage_copyfile_test_storage_device_type_validation"
    return 1;
  fi

  if [ -z "$destination_storage_device_type" ]
  then
    info_msg "ERROR: Destination storage device is not specified. It should be one of the following:"
    info_msg "onboard_flash, sdcard-HC, sdcard-XC, sdcard-regular, usb"
    report_fail "storage_copyfile_test_storage_device_type_validation"
    return 1;
  fi

  if_ligitimate_device_type=$(if_known_storage_device_type $destination_storage_device_type)
  if [ "$if_ligitimate_device_type" = false ]
  then
    info_msg "ERROR: Destination storage device is unknown. It should be one of the following:"
    info_msg "onboard_flash, sdcard-HC, sdcard-XC, sdcard-regular, usb"
    report_fail "storage_copyfile_test_storage_device_type_validation"
    return 1;
  fi
  
  if [ -z "$CONNECTED_DEVICES_NUM" ]
  then
    info_msg "Number of $destination_storage_device_type devices connected is not given, defaulting to $default_connected_devices_num"
    CONNECTED_DEVICES_NUM=$default_connected_devices_num
  fi
  
  if [ "$CONNECTED_DEVICES_NUM" -lt 1 ]
  then
    info_msg "ERROR: Number of connected devices can't be less then 1"
    report_fail "storage_copyfile_test_device_number_validation"
    return 1;
  fi

  if [ -z "$TEST_FILESIZE_IN_MB" ]
  then
    if [ -e "$pt_mode_file" ]; then
      info_msg "File size in MB, not supplied. Defaulting to $default_file_size_pt MB"
      TEST_FILESIZE_IN_MB=$default_file_size_pt
    else
      info_msg "File size in MB, not supplied. Defaulting to $default_file_size MB"
      TEST_FILESIZE_IN_MB=$default_file_size
    fi
  fi

  if [ -z "$SOURCE_TO_DESTINATION_KPI_THRESHOLD_MBPS" ]
  then 
    info_msg "Source to destination copy KPI MBps Threshold is not given, defaulting to $default_data_rate_kpi"
    SOURCE_TO_DESTINATION_KPI_THRESHOLD_MBPS=$default_data_rate_kpi
  fi

  if [ -z "$DESTINATION_TO_SOURCE_KPI_THRESHOLD_MBPS" ]
  then
    info_msg "Destination to source copy KPI MBps Threshold is not given, defaulting to $default_data_rate_kpi"
    DESTINATION_TO_SOURCE_KPI_THRESHOLD_MBPS=$default_data_rate_kpi
  fi

  if [ -z "$IF_MEASURE_THROUGHPUT" ]
  then
    if [ -e "$pt_mode_file" ]; then
      info_msg "If measure file copy throughput is not given, defaulting to $default_if_measure_throughput_pt"
      IF_MEASURE_THROUGHPUT=$default_if_measure_throughput_pt
    else
      info_msg "If measure file copy throughput is not given, defaulting to $default_if_measure_throughput"
      IF_MEASURE_THROUGHPUT=$default_if_measure_throughput
    fi
  fi
}

# checking if connected devices number given corresponds the real one, preparing list of devices and checking if mount should be done
get_devices_to_test() {
  local storage_device_type=$1
  local number_of_storage_devices_expected=$2
  local source_or_destination=$3
  local number_of_storage_devices_found=0
  
  info_msg "Storage Device Type : $storage_device_type"

  if [ "$storage_device_type" = "usb" ]; then
    prepare_usb_devices_list $source_or_destination
    if [ "$?" -ne 0 ]; then 
      report_fail "No_USB_device_is_plugged_in"
      return 1
    fi
    count_devices_found $source_or_destination
    number_of_storage_devices_found=$?
    if [ $number_of_storage_devices_found -ne $CONNECTED_DEVICES_NUM ]; then
      report_fail "Expected:_${number_of_storage_devices_expected}_USB_devices._Plugged:_${number_of_storage_devices_found}_USB_devices"
      return 1
    fi   
    eval IF_MOUNT_${source_or_destination}_STORAGE_DEVICE_IS_NEEDED=true
 
  elif echo $storage_device_type | grep "sdcard.*"; then
    prepare_sd_card_devices_list $source_or_destination
    if [ "$?" -ne 0 ]; then 
      report_fail "No_SD_card_device_is_plugged_in"
      return 1
    fi
    count_devices_found $source_or_destination
    number_of_storage_devices_found=$?
    if [ "$number_of_storage_devices_found" -ne "$CONNECTED_DEVICES_NUM" ]; then
      report_fail "Expected:_${number_of_storage_devices_expected}_SD_card_devices._Plugged:_${number_of_storage_devices_found}_SD_card_devices"
      return 1
    fi
    
    check_sd_card_filesystem_type "${source_or_destination}"
    if [ "$?" -ne 0 ]; then 
      report_fail "One_or_more_SD_card_devices_have_not_ext4_filesytem"
      return 1
    fi
    eval IF_MOUNT_${source_or_destination}_STORAGE_DEVICE_IS_NEEDED=true

  elif [ "$storage_device_type" = "onboard_flash" ]; then
    # currently there is only one onboard_flash as a storage device
    if [ "$number_of_storage_devices_expected" -ne 1 ]; then
      report_fail "Number_of_${storage_device_type}-evice_storages_has_to_be_1.__NOT_${number_of_storage_devices_expected}"
      return 1
    fi
    eval LIST_OF_${source_or_destination}_STORAGE_DEVICES="dummy_string_onboard_flash"
    eval IF_MOUNT_${source_or_destination}_STORAGE_DEVICE_IS_NEEDED=false
    
  else
    report_fail "ERROR: this line should not get executed. if it did, somebody changed the original working code"
    exit 1
  fi

  return 0
}

# mounting device if there is need to do so

mount_device()
{
  local if_to_mount=$1
  local device_name=$2
  local folder_to_mount=$3
  
  if [ "$if_to_mount" = "true" ]; then
    mount -o rw /dev/$device_name $folder_to_mount > /dev/null 2>&1
    exit_on_fail "mount_$device_name"
    info_msg "device mounted to $folder_to_mount"
  fi
}

# analyzing results of the test

analyze_test_results() {
  
  local file_size=$1
  local log_src_to_dest=$2
  local source_storage_device_type=$3
  local log_dest_to_src=$4
  local destination_storage_device_type=$5
  
  info_msg "File size: $file_size bytes"

  if [ $IF_MEASURE_THROUGHPUT = true ]; then
    local time_to_copy_from_src_to_dest=$( (cat $log_src_to_dest | grep real | awk '{print $2 }' ) )
    info_msg "Time took to copy file from $source_storage_device_type to $destination_storage_device_type: $time_to_copy_from_src_to_dest seconds"
    local time_to_copy_from_dest_to_src=$( (cat $log_dest_to_src | grep real | awk '{print $2 }' ) )
    info_msg "Time took to copy file from $destination_storage_device_type to $source_storage_device_type: $time_to_copy_from_dest_to_src seconds"

    local copy_data_rate_from_src_to_dest=$( (echo $file_size $time_to_copy_from_src_to_dest | awk '{ print $1 / $2 / 1024 / 1024 }' ) )
    local copy_data_rate_from_src_to_dest_INT=$( (awk "BEGIN {printf \"%d\n\",$copy_data_rate_from_src_to_dest}") )

    info_msg "Copy from $source_storage_device_type to $destination_storage_device_type data rate: $copy_data_rate_from_src_to_dest MBps"

    local copy_data_rate_from_dest_to_src=$( (echo $file_size $time_to_copy_from_dest_to_src | awk '{ print $1 / $2 / 1024 / 1024 }' ) )
    local copy_data_rate_from_dest_to_src_INT=$( (awk "BEGIN {printf \"%d\n\",$copy_data_rate_from_dest_to_src}") )

    info_msg "Copy from $destination_storage_device_type to $source_storage_device_type data rate: $copy_data_rate_from_dest_to_src MBps"

    info_msg "From: $source_storage_device_type, To: $destination_storage_device_type, KPI MBps Threshold: $SOURCE_TO_DESTINATION_KPI_THRESHOLD_MBPS MBps, Actual Rate from Test: $copy_data_rate_from_src_to_dest MBps"
    info_msg "From: $destination_storage_device_type, To: $source_storage_device_type, KPI MBps Threshold: $DESTINATION_TO_SOURCE_KPI_THRESHOLD_MBPS MBps, Actual Rate from Test: $copy_data_rate_from_dest_to_src MBps"

    local write_res_msg="Write_KPI_=_$SOURCE_TO_DESTINATION_KPI_THRESHOLD_MBPS-Mbps.__Measured_write_data_rate_=_$copy_data_rate_from_src_to_dest-Mbps" 

    if [ $copy_data_rate_from_src_to_dest_INT -lt $SOURCE_TO_DESTINATION_KPI_THRESHOLD_MBPS ]; then 
      report_fail "$write_res_msg"
      result=1
    else
      report_pass "$write_res_msg"
    fi

    local read_res_msg="Read_KPI_=_$DESTINATION_TO_SOURCE_KPI_THRESHOLD_MBPS.__Measured_data_rate_=_$copy_data_rate_from_dest_to_src-Mbps"

    if [ $copy_data_rate_from_dest_to_src_INT -lt $DESTINATION_TO_SOURCE_KPI_THRESHOLD_MBPS ]; then
      report_fail "$read_res_msg"
      result=1
    else
      report_pass "$read_res_msg"
    fi
  else
    report_pass "write-data"
    report_pass "read-data"
  fi
}

# run_storage_copyfile_test <StorageDeviceType> <TestFileSizeinMB> <WriteKPIThresholdMBps> <ReadKPIThresholdMBps>

run_storage_copyfile_test() {

  info_msg "Running Storage Copyfile Test"

  # default source for copy operation will be onboard_flash.
  local source_storage_device_type="onboard_flash"

  local destination_storage_device_type=$1
  TEST_FILESIZE_IN_MB=$2
  SOURCE_TO_DESTINATION_KPI_THRESHOLD_MBPS=$3
  DESTINATION_TO_SOURCE_KPI_THRESHOLD_MBPS=$4
  IF_MEASURE_THROUGHPUT=$5
  CONNECTED_DEVICES_NUM=$6
  
  analyze_input_params $source_storage_device_type $destination_storage_device_type
  exit_on_fail "Input_data_analysis"

  # preparing for test  
  LIST_OF_SOURCE_STORAGE_DEVICES="" # for now only one source storage device
  LIST_OF_DESTINATION_STORAGE_DEVICES=""

  # IF_MOUNT variables will be redefined in get_devices_to_test function on need basis
  IF_MOUNT_SOURCE_STORAGE_DEVICE_IS_NEEDED=false
  IF_MOUNT_DESTINATION_STORAGE_DEVICE_IS_NEEDED=false
  
  get_devices_to_test $source_storage_device_type $default_source_storage_device_count "SOURCE"
  exit_on_fail "checking_correct_count_of_storage_devices--source"

  get_devices_to_test $destination_storage_device_type $CONNECTED_DEVICES_NUM "DESTINATION"
  exit_on_fail "checking_correct_count_of_storage_devices--destination"
  
  local folder_src_to_dest='/home/linaro/folder_src_to_dest'
  local folder_dest_to_src='/home/linaro/folder_dest_to_src'
  local filename_src_to_dest=storage-copy-test.bin
  local filename_dest_to_src=storage-copy-test2.bin
  filename_roundtrip_local=storage-copy-test3.bin

  IF_FAIL=false

  # do some cleanup of potenitally previously failed run
  cleanup

  for i in $LIST_OF_DESTINATION_STORAGE_DEVICES
  do
    mkdir -p $folder_src_to_dest
    mkdir -p $folder_dest_to_src

    ####  mount  #####
    info_msg "Preparing folder for source to destination copy operation"
    mount_device $IF_MOUNT_SOURCE_STORAGE_DEVICE_IS_NEEDED $LIST_OF_SOURCE_STORAGE_DEVICES $folder_src_to_dest
    exit_on_fail "check-mounting-for-source-to-destination"
    info_msg "source storage device mount directory is:  $folder_src_to_dest"
  
    info_msg "Preparing folder for destination to source copy operation"
    mount_device $IF_MOUNT_DESTINATION_STORAGE_DEVICE_IS_NEEDED $i $folder_dest_to_src
    exit_on_fail "check-mounting-for-destination-to-source"
    info_msg "destination storage device mount directory is:  $folder_dest_to_src"
    
    ####  prepare files to copy #####
    info_msg "creating the temp $folder_src_to_dest/$filename_src_to_dest"
    dd if=/dev/urandom of=$folder_src_to_dest/$filename_src_to_dest bs=1M count=$TEST_FILESIZE_IN_MB conv=fsync
    sync

    info_msg "Clearning cache"
    echo 3 > /proc/sys/vm/drop_caches

    local log_src_to_dest="${OUTPUT}/copyfile_src_to_dest.log"
    local log_dest_to_src="${OUTPUT}/copyfile_dest_to_src.log"
    
    ####  copy  #####
    copy_file $folder_src_to_dest/$filename_src_to_dest $folder_dest_to_src/$filename_dest_to_src $log_src_to_dest $IF_MEASURE_THROUGHPUT
    exit_on_fail "check-copyfile-operation-src-to-dest"

    local file_size=$( (ls -l $folder_src_to_dest/$filename_src_to_dest | awk '{ print $5 }' ) )
    rm $folder_src_to_dest/$filename_src_to_dest

    copy_file $folder_dest_to_src/$filename_dest_to_src $folder_src_to_dest/$filename_roundtrip_local $log_dest_to_src $IF_MEASURE_THROUGHPUT
    exit_on_fail "check-copyfile-operation-dest-to-src"

    rm $folder_dest_to_src/$filename_dest_to_src
    rm $folder_src_to_dest/$filename_roundtrip_local
    
    ####  analyze results  #####
    info_msg "Analyzing data"
    analyze_test_results $file_size $log_src_to_dest $source_storage_device_type $log_dest_to_src $destination_storage_device_type
    
    cleanup
    
    if [ "$IF_FAIL" = "true" ]; then
      return 1
    fi
  done

  return 0
}
