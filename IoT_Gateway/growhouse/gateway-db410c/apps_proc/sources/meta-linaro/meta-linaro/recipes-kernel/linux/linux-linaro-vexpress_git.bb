DESCRIPTION = "Linaro Kernel For Vexpress/KVM"

SRCREV_kernel="0cf25829896330dcf8f95d8484c5f0eae6923f4f"
PV = "4.1+4.2rc+git${SRCPV}"
SRC_URI_append = " git://git.linaro.org/git/kernel/linux-linaro-tracking.git;protocol=http;branch=linux-linaro;name=kernel "
require linaro-kernel.inc

KERNEL_DEVICETREE = "vexpress-v2p-ca15-tc1.dtb"
BOOTARGS_COMMON = "root=/dev/mmcblk0p2 console=ttyAMA0 consolelog=9 mem=1024M rw rootwait"
COMPATIBLE_HOST = "arm.*"
KERNEL_IMAGETYPE = "zImage"

do_configure_prepend() {
    . ../ubuntu-ci/configs/vexpress.cfg
    ARCH=arm scripts/kconfig/merge_config.sh -m $linaro_base_config_frags $ubuntu_config_frag $board_config_frags
}
