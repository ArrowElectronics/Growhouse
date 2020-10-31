require linux.inc

DESCRIPTION = "Linux ILP32 kernel for HiKey"

PV = "4.12+git${SRCPV}"
SRCREV_kernel = "161da3d79f8b410ffdc5b57d8ee0b41db060cced"
SRCREV_FORMAT = "kernel"

SRC_URI = "git://git.kernel.org/pub/scm/linux/kernel/git/arm64/linux.git;protocol=https;branch=staging/ilp32-4.12;name=kernel \
"

S = "${WORKDIR}/git"

COMPATIBLE_MACHINE = "hikey-ilp32|genericarmv8-ilp32"
KERNEL_IMAGETYPE ?= "Image"

# EXTRA_OEMAKE += "V=1 ARCH=arm64 CROSS_COMPILE=aarch64-oe-linux-gnuilp32-"

# make[3]: *** [scripts/extract-cert] Error 1
DEPENDS += "openssl-native"
HOST_EXTRACFLAGS += "-I${STAGING_INCDIR_NATIVE}"

do_configure() {
    cp ${S}/arch/arm64/configs/defconfig ${B}/.config

    # Make sure to enable NUMA
    echo 'CONFIG_NUMA=y' >> ${B}/.config

    # Check for kernel config fragments. The assumption is that the config
    # fragment will be specified with the absolute path. For example:
    #   * ${WORKDIR}/config1.cfg
    #   * ${S}/config2.cfg
    # Iterate through the list of configs and make sure that you can find
    # each one. If not then error out.
    # NOTE: If you want to override a configuration that is kept in the kernel
    #       with one from the OE meta data then you should make sure that the
    #       OE meta data version (i.e. ${WORKDIR}/config1.cfg) is listed
    #       after the in-kernel configuration fragment.
    # Check if any config fragments are specified.
    if [ ! -z "${KERNEL_CONFIG_FRAGMENTS}" ]; then
        for f in ${KERNEL_CONFIG_FRAGMENTS}; do
            # Check if the config fragment was copied into the WORKDIR from
            # the OE meta data
            if [ ! -e "$f" ]; then
                echo "Could not find kernel config fragment $f"
                exit 1
            fi
        done

        # Now that all the fragments are located merge them.
        ( cd ${WORKDIR} && ${S}/scripts/kconfig/merge_config.sh -m -r -O ${B} ${B}/.config ${KERNEL_CONFIG_FRAGMENTS} 1>&2 )
    fi

    yes '' | oe_runmake -C ${S} O=${B} oldconfig

    bbplain "Saving defconfig to:\n${B}/defconfig"
    oe_runmake -C ${B} savedefconfig
}

do_deploy_append() {
    cp -a ${B}/defconfig ${DEPLOYDIR}
    cp -a ${B}/.config ${DEPLOYDIR}/config
}
