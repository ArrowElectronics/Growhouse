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

####################################################################################
### description: The DUT makes himself discoverable. HELPER scan BT devices and finds DUT in scan logs.
####################################################################################

############################# function definition #######################################


copy_util_file_to_HELPER()
{
	local file_to_copy_path="$1"
	scp $SSH_PARAMS $file_to_copy_path root@$HELPER_IP_ADDR: 
}

remove_util_file_from_HELPER()
{
	local file_to_remove="$1"
	ssh $SSH_PARAMS root@$HELPER_IP_ADDR "rm $file_to_remove"
}

make_discoverable_cleanup()
{
	remove_util_file_from_HELPER $EXPECT_LIB_UTIL
	remove_util_file_from_HELPER $HELPER_SCAN_FOR_DUT_UTIL

	# close_BT function defined in BT-sh-lib.sh	
	close_BT
}
############################# activating relevant libraries #######################################

# test framework functions
. ../../lib/sh-test-lib
. ../../lib/set-result-dir.sh

# configuration variables for all BT tests
. ../../utils/BT-tests-util/BT-tests-config.sh
# functions for all BT tests
. ../../utils/BT-tests-util/BT-sh-lib.sh

############################# script execution #######################################

../../utils/BT-tests-util/ssh-authenticate.sh $HELPER_IP_ADDR


trap make_discoverable_cleanup EXIT

#bring up DUT
change_BT_status "up"

#bring up HELPER
change_BT_status "up" $HELPER_IP_ADDR

make_discoverable "DUT"
sleep 10

EXPECT_LIB_UTIL="BT-expect-lib.sh"
HELPER_SCAN_FOR_DUT_UTIL="HELPER_scan_for_DUT.sh"

copy_util_file_to_HELPER "../../utils/BT-tests-util/$EXPECT_LIB_UTIL"
copy_util_file_to_HELPER "util/$HELPER_SCAN_FOR_DUT_UTIL"

# arguments are set in BT-sh-lib.sh and in BT-tests-config.sh
ssh $SSH_PARAMS root@$HELPER_IP_ADDR "./HELPER_scan_for_DUT.sh $DUT_MAC_ADDR $DEBUG_OPTION $OUTPUT_OPTION"
check_return "BT-make-discoverable"

