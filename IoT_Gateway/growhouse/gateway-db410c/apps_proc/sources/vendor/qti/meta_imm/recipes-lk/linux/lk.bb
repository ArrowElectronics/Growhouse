PATCHTOOL = "git"
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

DESCRIPTION = "LK for MSM platforms"
LICENSE = "BSD"
OVERRIDES_append = ":${LK_BUILD_FLAVOR}"
LIC_FILES_CHKSUM = "file://../git/LICENSE;md5=5a1abdab641eec675725c843f43f03af"
LIC_FILES_CHKSUM_local = "file://../lk/LICENSE;md5=5a1abdab641eec675725c843f43f03af"

inherit externalsrc

EXTERNALSRC_local = "${TOPDIR}/../sources/lk"
SRC_URI_user = "${USER_SRC_URI_LK}"
SRCREV_user = "${USER_SRCREV_LK}"
SRC_URI = "git://${TOPDIR}/../sources/lk;protocol=file;branch=development;name=lk"
SRCREV = "development"

SRC_URI += "file://0001-Gateway-410C-Remove-switching-to-emmc-partition.patch \
            file://0002-Gateway-DB410C-MAC-address-support-lk.patch \
            "

COMPATIBLE_MACHINE = "(dragonboard-410c|dragonboard-820c)"

S = "${WORKDIR}/git"

TOOLCHAIN_PREFIX_PATH = "${TOPDIR}/../arm-eabi-4.8/bin/arm-eabi-"

do_configure() {
	echo "DO NOTHING"
}

do_compile () {
	oe_runmake -C ${S} -j4 msm8916 EMMC_BOOT=1 TOOLCHAIN_PREFIX=${TOOLCHAIN_PREFIX_PATH} LD=${TOOLCHAIN_PREFIX_PATH}ld
	mkdir -p ${DEPLOY_DIR}/images/${MACHINE}
	cp ${S}/build-msm8916/emmc_appsboot.mbn ${DEPLOY_DIR}/images/${MACHINE}
}
