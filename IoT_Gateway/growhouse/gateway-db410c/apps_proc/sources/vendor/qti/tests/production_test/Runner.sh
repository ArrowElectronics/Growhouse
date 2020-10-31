#!/bin/sh
#Copyright (c) 2018, The Linux Foundation. All rights reserved.
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

PRODUCTION_MODE_FILE="test-definitions/automated/lib/storage/pt_mode_is_on"
OUTPUT_PATH="/root/output"
RESULTS_FILE="$OUTPUT_PATH/result.csv"
TEMP_RESULT_FILE="$OUTPUT_PATH/temp.txt"
TEST_RESULT_TABLE="$OUTPUT_PATH/result_table.txt"
FINAL_OUTPUT=0

clean_results() {
  rm $RESULTS_FILE
}

clean_test_table() {
  rm $TEMP_RESULT_FILE
  rm $TEST_RESULT_TABLE
}

exit_if_fail() {
  local last_test="$1"
  local fail_res="$(cat $RESULTS_FILE | grep fail)"
  
  if [ ! "$fail_res" = "" ]; then
    echo "$last_test FAILED"
    read line
    exit 1
  fi
}

save_results() {
  local last_test="$1"
  
  if [ ! -f "$RESULTS_FILE" ]; then
    echo "$last_test \$ FAIL" >> $TEMP_RESULT_FILE
    return
  fi
  
  local all_results="$(cat $RESULTS_FILE)"
  if [ "$all_results" = "" ]; then
    echo "$last_test \$ FAIL" >> $TEMP_RESULT_FILE
    return
  fi
  
  local fail_res="$(cat $RESULTS_FILE | grep fail)"
  if [ ! "$fail_res" = "" ]; then
    echo "$last_test \$ FAIL" >> $TEMP_RESULT_FILE
  else
    echo "$last_test \$ PASS" >> $TEMP_RESULT_FILE
  fi
}

prepare_results() {
  local unset final_if_fail
  column -t -s'\$' $TEMP_RESULT_FILE > $TEST_RESULT_TABLE
  rm $TEMP_RESULT_FILE
  
  final_if_fail=$(grep -i fail $TEST_RESULT_TABLE)
  if [ "$final_if_fail" = "" ]; then
    FINAL_OUTPUT=0
  else
    FINAL_OUTPUT=1
  fi
}


  touch $PRODUCTION_MODE_FILE
  clean_test_table
  mkdir /root/output
  echo "STARTING TESTING PROCESS"
  
  cd test-definitions
  . automated/bin/setenv.sh

  clean_results
  current_test="USB MASS STORAGE ENUMARTION TEST"
  echo ""
  echo "$current_test"
  test-runner -d automated/linux/usb/USB-storage-enumeration.yaml
  save_results "$current_test"
  echo "===================================================================="

  clean_results
  current_test="USB DEVICE ENUMARTION TEST"
  echo ""
  echo "$current_test"
  test-runner -d automated/linux/usb/USB-any-enumeration.yaml
  save_results "$current_test"
  echo "===================================================================="

  clean_results
  current_test="USB 2 MASS STORAGE COPYFILE TEST"
  echo ""
  echo "$current_test"
  test-runner -d automated/linux/storage/copyfile/usb-mass-storage-copyfile.yaml
  save_results "$current_test"
  echo "===================================================================="

  clean_results
  current_test="SD CARD STORAGE COPYFILE TEST"
  echo ""
  echo "$current_test"
  test-runner -d automated/linux/storage/copyfile/sdcard-hc-copyfile.yaml
  save_results "$current_test"
  echo "===================================================================="
  
  clean_results
  current_test="ETHERNET TEST"
  echo ""
  echo "$current_test"
  test-runner -d automated/linux/ethernet/ethernet.yaml
  save_results "$current_test"
  echo "===================================================================="
  
  clean_results
  current_test="WIFI TEST"
  echo ""
  echo "$current_test"
  test-runner -d automated/linux/wifi-STA-secure-connect/wifi-STA-secure-connect.yaml
  save_results "$current_test"
  echo "===================================================================="
  
  clean_results
  current_test="I2C AND GPIO TEST"
  echo ""
  echo "$current_test"
  test-runner -d automated/linux/i2c/i2c_820.yaml
  save_results "$current_test"
  echo "===================================================================="
  
  clean_results
  current_test="BLUETOOTH TEST"
  echo ""
  echo "$current_test"
  test-runner -d automated/linux/BTLE/BTLE.yaml
  save_results "$current_test"
  echo "===================================================================="

  echo "####################################################################"
  echo "                        STARTING MANUAL TESTS                       "
  echo "####################################################################"

  cd ../manual_tests

  clean_results
  current_test="VOLUME UP TEST"
  echo ""
  echo "$current_test"
  ./volume_up.sh "$RESULTS_FILE"
  save_results "$current_test"
  echo "===================================================================="

  # This test is temporally disabled because the volume down button
  # doesn't work on Linaro Debian build at the present time
  #clean_results
  #current_test="VOLUME DOWN TEST"
  #echo ""
  #echo "$current_test"
  #./volume_down.sh "$RESULTS_FILE"
  #save_results "$current_test"
  #echo "===================================================================="
  
  cd ..
  
  # summarize and report testing results
  prepare_results
  sleep 1
  xterm -hold -e "./ReportResults.sh $TEST_RESULT_TABLE" > /dev/null 2>&1 &
  read line
  sleep 1
  rm -r "$OUTPUT_PATH"

exit $FINAL_OUTPUT
