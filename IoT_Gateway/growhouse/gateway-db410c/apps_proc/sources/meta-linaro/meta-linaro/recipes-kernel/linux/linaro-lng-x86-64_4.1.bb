DESCRIPTION = "Linaro LNG Kernel For x86"
PV = "4.1+git${SRCPV}"

require linaro-lng.inc

SRCREV = "b00df57e50e087151f4ca976ca2796ea105f68c1"

SRC_URI = "git://git.linaro.org/kernel/linux-linaro-lng.git;branch=linux-linaro-lng-v4.1"

do_configure_prepend() {
    cd ${S}
    scripts/kconfig/merge_config.sh -m arch/x86/configs/x86_64_defconfig \
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
