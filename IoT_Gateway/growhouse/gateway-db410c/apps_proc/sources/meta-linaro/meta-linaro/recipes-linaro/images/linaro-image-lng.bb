require linaro-image-common.inc

# Override method in linaro-image-common.inc
do_systemd_network () {
    echo "Do nothing"
}

inherit ${@bb.utils.contains_any("MACHINE", [ "qemux86", "qemux86-64" ], "image-vm", "base", d)}

IMAGE_INSTALL += " \
    arndale-pre-boot \
    bridge-utils \
    calibrator \
    cunit \
    curl \
    cronie \
    fping \
    git \
    libhugetlbfs \
    libvirt \
    libvirt-libvirtd \
    libvirt-virsh \
    lmbench \
    lng-network-config \
    ltp \
    netperf \
    odp \
    openssh-sftp-server \
    openvswitch \
    packagegroup-core-buildessential \
    procps \
    python-numpy \
    qemu \
    rt-tests \
    trace-cmd \
    tunctl \
    usecpu \
    "

# install the odp-ptest package if ptest is enabled in the
# DISTRO_FEATURES (configured in local.conf)
IMAGE_INSTALL += "${@bb.utils.contains( \
                  'DISTRO_FEATURES', 'ptest', 'odp-ptest', '', d)}"

IMAGE_INSTALL_append_armv7a = " \
    latency-test \
    systemtap \
    trinity-example \
    "

IMAGE_INSTALL_append_aarch64 = " \
    trinity-example \
    "

IMAGE_INSTALL_append_qemux86 = " \
    "

IMAGE_FEATURES += "\
    dev-pkgs \
    staticdev-pkgs \
    tools-debug \
    tools-sdk \
    "
IMAGE_FSTYPES_append_qemux86 += "cpio.gz qcow2"
IMAGE_FSTYPES_append_qemux86-64 += "cpio.gz qcow2"
IMAGE_FSTYPES_lng-x86-64 = "tar.gz cpio.gz"
IMAGE_FSTYPES_lng-rt-x86-64 = "tar.gz cpio.gz"

EXTRA_IMAGE_FEATURES_append_qemux86 = " autoserial"
FEATURE_PACKAGES_autoserial = "auto-serial-console"

IMAGE_PREPROCESS_COMMAND_qemux86 += "qemux86_fixup;"
IMAGE_PREPROCESS_COMMAND_qemux86-64 += "qemux86_fixup;"

qemux86_fixup() {
        # The hostname can be changed by using
        # hostname_pn-base-files = "linaro"
        # See base-files recipe
        echo "linaro" > ${IMAGE_ROOTFS}${sysconfdir}/hostname
}
