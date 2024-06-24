DESCRIPTION = "Linaro LNG Kernel For x86"
PV = "4.1+git${SRCPV}"

require linaro-lng.inc

SRCREV = "48ffd0d5e7b2fb6e7d47344320dbeeaf15d6fd5a"

SRC_URI = "git://git.linaro.org/kernel/linux-linaro-lng.git;branch=linux-linaro-lng-v4.1-rt"

do_configure_prepend() {
    cd ${S}
    scripts/kconfig/merge_config.sh -m arch/x86/configs/x86_64_defconfig \
     linaro/configs/preempt-rt.conf \
     linaro/configs/ovs.conf \
     linaro/configs/kvm-host.conf \
     linaro/configs/kvm-guest.conf \
     linaro/configs/no_hz_full.conf
    scripts/config -e CONFIG_DEVTMPFS
    scripts/config -e CONFIG_DEVTMPFS_MOUNT
    scripts/config -e CONFIG_E1000E
    scripts/config -e CONFIG_IXGBE
    cd ${B}
}
