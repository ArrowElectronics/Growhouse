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


# This file allows the tester to set values for all variables involved on the tests
# There's no need to change variables which will not be used on the test


#ssh parameters
#all ssh commands in BT tests use this variable as their ssh parameters
#if the user wants to add ssh parameters he can do so by adding them to SSH_PARAMS variable.
#e.g. SSH_PARAMS=-o StrictHostKeyChecking=no -o ConnectTimeout=5
#default is: SSH_PARAMS=-o StrictHostKeyChecking=no
#SSH_PARAMS="-o StrictHostKeyChecking=no -o ForwardX11Trusted=yes"
SSH_PARAMS="-o StrictHostKeyChecking=no"

#######################################################################################

# IP address -> HELPER Device
HELPER_IP_ADDR="10.18.128.29"
# user login -> HELPER device
HELPER_USER_NAME="linaro"
# user password -> HELPER device
HELPER_USER_PASSWORD="linaro"


#######################################################################################

# Channel for data transmission via bluetooth
CHANNEL_OPP=14

#######################################################################################

# Debug option must be set as 1 to turn on the debbug mode or 0 to turn it off
# 1 = Debbug mode on
# 0 = Debug mode off
DEBUG_OPTION=0

#######################################################################################

# Out option must be set as 1 to show all output in STDOUT and 0 to not do it
# 1 = output to STDOUT on
# 0 = output to STDOUT off
OUTPUT_OPTION=0
