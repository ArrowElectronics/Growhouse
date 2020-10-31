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

report_final_result() {
  local final_res="$1"
  
  echo ""
  echo "#############################################################"
  echo ""
  
  if [ "$final_res" = "pass" ]; then
    echo "   ########         ##         ########   ########   "
    echo "   ##    ##        ####        ##         ##         "
    echo "   ##    ##       ##  ##       ##         ##         "
    echo "   ########      ##    ##      ########   ########   "
    echo "   ##           ##########           ##         ##   "
    echo "   ##          ##        ##          ##         ##   "
    echo "   ##         ##          ##   ########   ########   "
  elif [ "$final_res" = "fail" ]; then
    echo "   ########         ##         ##   ##         "
    echo "   ##              ####        ##   ##         "
    echo "   ##             ##  ##       ##   ##         "
    echo "   ########      ##    ##      ##   ##         "
    echo "   ##           ##########     ##   ##         "
    echo "   ##          ##        ##    ##   ##         "
    echo "   ##         ##          ##   ##   ########   "
  fi
  
  echo ""
  echo "#############################################################"
  echo ""
}

report_results() {
  local results_file="$1"
  local final_if_fail=""
  final_if_fail=$(grep -i fail $results_file)
  if [ "$final_if_fail" = "" ]; then
    local final_res="pass"
  else
    local final_res="fail"
  fi
  
  report_final_result "$final_res"
  cat $results_file
}

if [ -z "$1" ]; then
  echo "No results file"
else
  report_results "$1"
fi
