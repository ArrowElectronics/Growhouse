#Copyright (c) 2016, The Linux Foundation. All rights reserved.
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


#!/bin/sh
# script installs desired package with opkg on OE build


PKG_NAME=$1

echo "=============== starting install-pkg.sh  ======================="
SOURCE="http://10.18.167.14/ipk"
OPKG_CONF=/etc/opkg/opkg.conf

# check opkg binaries present
which opkg || ( echo "no binaries of opkg. script fails" && exit 1 ) || exit 1 

# if package already installed leave script successfully
opkg list-installed | grep $PKG_NAME && echo "=== package already installed! successfully finished install-pkg.sh  ====" && exit 0

touch /etc/init.d/ifup
grep "$SOURCE" $OPKG_CONF || echo "src/gz OE_OPKG $SOURCE" >> $OPKG_CONF
opkg update || ( echo "======> opkg install failed to update! <=======" && exit 1 ) || exit 1

# check package in source
opkg list | grep $PKG_NAME || ( echo "package $PKG_NAME not in source $SOURCE!!! script fails" && exit 1 ) || exit 1 

opkg install $PKG_NAME || ( echo "======> opkg install failed to install! <=======" && exit 1 ) || exit 1

sleep 2

opkg list-installed | grep $PKG_NAME || ( echo "======> opkg install failed! <=======" && exit 1 ) || exit 1 

echo "=============== successfully finished install-pkg.sh  ======================="
exit 0


