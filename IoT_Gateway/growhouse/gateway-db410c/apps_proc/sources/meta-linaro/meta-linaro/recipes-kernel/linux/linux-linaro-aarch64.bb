DESCRIPTION = "Linaro recipe for Aarch64 fast model kernels"

SRCREV_kernel="0cf25829896330dcf8f95d8484c5f0eae6923f4f"
PV = "4.1+4.2rc+git${SRCPV}"

SRC_URI_append = " git://git.linaro.org/git/kernel/linux-linaro-tracking.git;protocol=http;name=kernel;nobranch=1 "
DEPENDS_append_aarch64 = " libgcc"
KERNEL_CC_append_aarch64 = " ${TOOLCHAIN_OPTIONS}"
KERNEL_LD_append_aarch64 = " ${TOOLCHAIN_OPTIONS}"


include linaro-kernel.inc

COMPATIBLE_HOST = "aarch64"
KERNEL_IMAGETYPE = "Image"
BOOTARGS_COMMON = "console=ttyAMA0 mem=2048M devtmpfs.mount=1 earlyprintk=pl011,0x1c090000 consolelog=9 rw"

do_configure_prepend() {
    . ../ubuntu-ci/configs/vexpress64.cfg
    cd ${S}
    ARCH=arm64 scripts/kconfig/merge_config.sh -m $linaro_base_config_frags $ubuntu_config_frag $board_config_frags
    cd ${B}
}
