PATCHTOOL = "git"
inherit externalsrc

FILESEXTRAPATHS_prepend := "${THISDIR}/patches:"

SRC_URI += "\
        file://0001-remoteproc-qcom-don-t-autoboot-Hexagon.patch \
        file://0002-arm64-defconfig-Enable-QRTR-features.patch \
        file://0003-drivers-soc-qcom-add-gps-proxy.patch \
        file://0004-msm-ADSPRPC-Enable-ADSP-RPC-driver.patch \
        file://0005-arm64-dts-msm8916-enable-hexagon-node.patch \
        file://0001-rtc-rtc-pm8xxx-Add-RTC-offset-file-support.patch \
        file://0005-kernel-configs-enable-GPS-proxy.patch \
        file://0001-wcn36xx-Add-support-for-FTM.patch \
        file://0001-kernel-configs-set-CONFIG_NL80211_TESTMODE-y.patch \
        file://0001-Added-RPMSG-driver-support.patch \
        file://0002-Gateway-DB410C-Added-support-of-CYUSB3610-chip.patch \
        file://0003-Gateway-DB410C-Support-for-Factory-reset-gpio.patch \
        file://0004-Gateway-DB410C-CYUSB3610-reset-pin.patch \
        file://0005-Gateway-DB410C-Added-cyusb-MAC-address-support.patch \
        file://0006-Gateway-DB410C-Removed-wifi-support-from-DB410C.patch \
        file://0007-Gateway-DB410C-SDC-clocks-runtime-enable-disable.patch \
        file://0008-Gateway-DB410C-Removed-unused-interfaces-support.patch \
        file://0009-Watchdog-Enable-pimic-watchdog.patch \
        "

OVERRIDES_append = ":${LINUX_BUILD_FLAVOR}"
EXTERNALSRC_local = "${TOPDIR}/../sources/kernel"
EXTERNALSRC_BUILD_local = "${TOPDIR}/../sources/kernel"
LINUX_LINARO_QCOM_GIT = "git://${TOPDIR}/../sources/kernel;protocol=file"
SRCBRANCH_local = "development"
SRCBRANCH_user = "development"
SRCBRANCH_caf = "development"
SRCBRANCH = "development"
SRCREV = "088455d317b09d6cb988526db3e454bb50d74f2d"
