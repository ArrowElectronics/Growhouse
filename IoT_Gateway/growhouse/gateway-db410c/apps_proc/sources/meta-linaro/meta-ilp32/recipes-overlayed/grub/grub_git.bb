#require recipes-bsp/grub/grub2.inc
LICENSE = "GPLv3"

LIC_FILES_CHKSUM = "file://COPYING;md5=ed1dca40ee0852c630f19c06fdecf6bc \
"

inherit deploy

DEFAULT_PREFERENCE = "-1"
DEFAULT_PREFERENCE_arm = "1"

FILESEXTRAPATHS =. "${FILE_DIRNAME}/grub-git:"

PV = "2.00-prebuilt"

COMPATIBLE_HOST = '(x86_64.*|i.86.*|arm.*|aarch64.*)-(linux.*|freebsd.*)'

# grubaa64.efi needs to be compiled in 64bit mode. grub compilation references userspace header
# files but in ILP32 build we don't have 64bit specific headers which results in build failure.
# Workaround by using a prebuilt grub efi application.
do_install() {
    wget -q http://releases.linaro.org/reference-platform/embedded/hikey/16.12/rpb/grubaa64.efi -O ${B}/grubaa64.efi
}

do_deploy() {
    cp -a ${B}/grubaa64.efi ${DEPLOYDIR}
}

addtask deploy before do_build after do_install
