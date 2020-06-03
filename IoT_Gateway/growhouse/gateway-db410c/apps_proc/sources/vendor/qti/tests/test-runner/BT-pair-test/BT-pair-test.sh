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
### description: The DUT pairs to the HELPER. The test is successful if the pairing is completed.
####################################################################################

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

# close_BT function defined in BT-sh-lib.sh
trap close_BT EXIT

#bring up DUT
change_BT_status "up"

#bring up HELPER
change_BT_status "up" $HELPER_IP_ADDR

make_discoverable "HELPER"
sleep 20

# these argument are set in BT-sh-lib.sh and in BT-tests-config.sh
util/pairing_from_DUT_to_HELPER.sh $HELPER_MAC_ADDR $DEBUG_OPTION $OUTPUT_OPTION
check_return "BT-pair"

