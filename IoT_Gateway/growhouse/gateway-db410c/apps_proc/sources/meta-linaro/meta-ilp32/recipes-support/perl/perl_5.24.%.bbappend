do_configure_append() {
    # Fixups for ILP32
    # This is to do away with LIB_VERSION, no longer defined
    # by math.h in recent versions of GLIBC.
    if echo "${TARGET_OS}" | grep -q '^linux.*ilp32$'; then
        sed -i -e "s,\(d_libm_lib_version=\)'define',\1'undef',g" \
            config.sh-${TARGET_ARCH}-${TARGET_OS}
    fi
}
