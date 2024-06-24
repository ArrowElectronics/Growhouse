#!/bin/bash

UNDER='\e[4m'
RED='\e[31;1m'
GREEN='\e[32;1m'
YELLOW='\e[33;1m'
BLUE='\e[34;1m'
MAGENTA='\e[35;1m'
CYAN='\e[36;1m'
WHITE='\e[37;1m'
ENDCOLOR='\e[0m'

PWD=`pwd`
BUILD_NUM=IMM.LE.1.0-8x16_40300
CURRENT_DIR=$PWD
BUILD=build
SETUP_ENV=setup-environment
BUILD_IMAGE=rpb-console-image
MACHINE=dragonboard-410c
BRANCHNAME=development
NEW_BRANCHNAME=development
BUILD_SOURCE=apps_proc
BUILDROOT=$BUILD_SOURCE


build_source(){
	cd $BUILD_SOURCE

	export OE_MACHINE="$MACHINE"
	export BUILD_NUMBER=$BUILD_NUM
	export IMAGES="$BUILD_IMAGE"
	QCOM_EULA=1 MACHINE=${OE_MACHINE}
	source $SETUP_ENV $BUILD
	echo "IMAGE_NAME_append = \"-${BUILD_NUMBER}\"" >> conf/local.conf
	echo "LICENSE_FLAGS_WHITELIST = \"commercial\"" >> conf/local.conf
	echo "ACCEPT_EULA_dragonboard-410c = \"1\"" >> conf/local.conf
	echo "MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS += \" lk\"" >> conf/local.conf
	echo "SOURCE_MIRROR_URL ?= \"file://${TOPDIR}/../downloads/\"" >> conf/local.conf
	echo "INHERIT += \"own-mirrors\"" >> conf/local.conf
	echo "BB_GENERATE_MIRROR_TARBALLS = \"1\"" >> conf/local.conf
	# BB_NO_NETWORK will turn off the network access during build.
	# echo "BB_NO_NETWORK = \"1\"" >> conf/local.conf
	sed -i '/PACKAGECONFIG_append_pn-qemu-native = \" sdl\"/d' conf/local.conf
	sed -i '/PACKAGECONFIG_append_pn-nativesdk-qemu = \" sdl\"/d' conf/local.conf

	bitbake ${IMAGES}
	if [ $? -eq 0 ]; then
		echo -e "$BLUE Build completed successfully"
		echo -e "$ENDCOLOR"
	else
		echo -e "$RED ERROR: Build error"
		echo -e "$ENDCOLOR"
		exit 1
	fi
}


check_program() {
	for cmd in "$@"
	do
        	which ${cmd} > /dev/null 2>&1
        	if [ $? -ne 0 ]
        	then
                	echo
                	echo -e "$RED Cannot find command \"${cmd}\" $ENDCOLOR"
			echo -e "$RED Install \"${cmd}\" $ENDCOLOR"
                	echo
                	exit 1
        	fi
	done
}

check_program git sshpass

if [ ! -d "$BUILD_SOURCE" ]; then
    mkdir -p $BUILD_SOURCE
fi

build_source
