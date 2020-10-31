#!/bin/bash
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

# NAME
#   thermal-conf.sh
#
# SYNOPSIS
#   . ./lib/thermal-conf.sh
#
# DESCRIPTION
#   Script saves general file path's for thermal test
#   If developer changes directory name or file path he can change it in one place.
#
#   NOTE: you need to include this lib at the begining in files that need these variables.
#

# Default dhrystone path
DHRY_PATH=bin/dhry

# Get output path beacuse only result file is exported.
OUTPUT=$(dirname "${RESULT_FILE}")

DHRY_LOG1="${OUTPUT}/dhry_on_cpu_1.log"
DHRY_LOG2="${OUTPUT}/dhry_on_cpu_2.log"
DHRY_LOG3="${OUTPUT}/dhry_on_cpu_3.log"
DHRY_LOG4="${OUTPUT}/dhry_on_cpu_4.log"
# Path to the dhrystone log summary
DHRY_LOG_SHORT="${OUTPUT}/dhry_all_CPUs_summary.log"
# Path to the full temperatures and CPU frequencies file results
LOG_FILE="${OUTPUT}/thermal_zones_and_CPU_freq.log"

# Temp file to store last time iterations
TIME_ITERATION_TEMP="/tmp/thermal_last_position.txt"

# Default dhrystone test score average
AVERAGE=0

# Max temperature for overheating
#MAX_TEMP=100000
MAX_TEMP=65000

# Default number of runs for dhrystone
NUMBER_OF_RUNS="500000000000000"

# Default test iterations
ITER=5

# Iteration counter.n
TIME_ITERATION=0
