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

PASSWORD="y"
WORKING_DIR="test-definitions"
IMAGE_FILE="picture.jpg"

sudo chmod 777 $WORKING_DIR -R
sudo echo -e "$PASSWORD\n$PASSWORD" | (sudo passwd root)


echo ""
echo ""
echo "In order to start the testing process insure that the board,"
echo "you are going to test, looks like one on the picture."
echo "All devices that you can see on the picture have to be connected"
echo "EXACTLY in the way they are on the picture. Otherwise you"
echo "could receive false negative results."
echo ""
echo "If something looks different, switch the board off, make"
echo "appropriate change, then turn the board on and run the script."
echo ""
echo "If the board looks like on the picture, after word \"Password: \"" 
echo "enter [$PASSWORD] and press the \"Enter\" button"
echo ""
echo ""

# open device picture and scheme
pcmanfm-qt $IMAGE_FILE > /dev/null 2>&1

su -c ./Runner.sh root
