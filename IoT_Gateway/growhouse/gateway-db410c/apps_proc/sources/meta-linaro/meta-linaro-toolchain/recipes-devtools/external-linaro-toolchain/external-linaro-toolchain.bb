require recipes-core/glibc/glibc-package.inc

INHIBIT_DEFAULT_DEPS = "1"
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"

# License applies to this recipe code, not the toolchain itself
LICENSE = "MIT"
LIC_FILES_CHKSUM = "\
	file://${COREBASE}/LICENSE;md5=4d92cd373abda3937c2bc47fbc49d690 \
	file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420 \
"

PROVIDES += "\
	linux-libc-headers \
	virtual/${TARGET_PREFIX}gcc \
	virtual/${TARGET_PREFIX}g++ \
	virtual/${TARGET_PREFIX}gcc-initial \
	virtual/${TARGET_PREFIX}binutils \
	virtual/${TARGET_PREFIX}libc-for-gcc \
	virtual/${TARGET_PREFIX}compilerlibs \
	virtual/libc \
	virtual/libintl \
	virtual/libiconv \
        glibc-mtrace \
	glibc-thread-db \
	glibc \
	libc-mtrace \
	gcc-runtime \
	libgcc \
	libg2c \
	libg2c-dev \
	libssp \
	libssp-dev \
	libssp-staticdev \
	libgfortran \
	libgfortran-dev \
	libgfortran-staticdev \
	libmudflap \
	libmudflap-dev \
	libgomp \
	libgomp-dev \
	libgomp-staticdev \
	libitm \
	libitm-dev \
	libitm-staticdev \
	libquadmath \
	libquadmath-dev \
	libquadmath-staticdev \
	virtual/linux-libc-headers \
	libgcov-dev \
"

PV = "${ELT_VER_MAIN}"

# https://launchpad.net/linaro-toolchain-binaries
SRC_URI = "file://SUPPORTED"

do_install() {
	# Add stubs for files OE-core expects
	install -d ${S}/nscd/
	touch  ${S}/nscd/nscd.init
	touch  ${S}/nscd/nscd.conf
	touch  ${S}/nscd/nscd.service

	install -d ${D}${base_libdir}
	install -d ${D}${bindir}
	install -d ${D}${sbindir}
	install -d ${D}${libdir}
	install -d ${D}${libexecdir}
	install -d ${D}${datadir}
	install -d ${D}${includedir}

	cp -a ${EXTERNAL_TOOLCHAIN}/${ELT_TARGET_SYS}/lib/*  ${D}${base_libdir}
	if [ -d ${EXTERNAL_TOOLCHAIN}/${ELT_TARGET_SYS}/libc/lib/${ELT_TARGET_SYS} ]; then
		cp -a ${EXTERNAL_TOOLCHAIN}/${ELT_TARGET_SYS}/libc/lib/${ELT_TARGET_SYS}/*  ${D}${base_libdir}
	else
		if [ -f ${EXTERNAL_TOOLCHAIN}/${ELT_TARGET_SYS}/libc/lib/ld-${ELT_VER_LIBC}.so ]; then
			cp -a ${EXTERNAL_TOOLCHAIN}/${ELT_TARGET_SYS}/libc/lib/*  ${D}${base_libdir}
		else
			cp -a ${EXTERNAL_TOOLCHAIN}/${ELT_TARGET_SYS}/libc/usr/lib/*.so*  ${D}${base_libdir}
		fi
	fi
	if [ -d ${EXTERNAL_TOOLCHAIN}/${ELT_TARGET_SYS}/libc/usr/lib/${ELT_TARGET_SYS} ]; then
		cp -a ${EXTERNAL_TOOLCHAIN}/${ELT_TARGET_SYS}/libc/usr/lib/${ELT_TARGET_SYS}/*  ${D}${libdir}
	else
		cp -a ${EXTERNAL_TOOLCHAIN}/${ELT_TARGET_SYS}/libc/usr/lib/*  ${D}${libdir}
		if [ ! -f ${EXTERNAL_TOOLCHAIN}/${ELT_TARGET_SYS}/libc/lib/ld-${ELT_VER_LIBC}.so ]; then
			rm -rf ${D}${libdir}/*.so*
		fi
	fi
	cp -a ${EXTERNAL_TOOLCHAIN}/${ELT_TARGET_SYS}/libc/usr/share/*  ${D}${datadir}
	cp -a ${EXTERNAL_TOOLCHAIN}/${ELT_TARGET_SYS}/libc/usr/include/*  ${D}${includedir}
	if [ -d ${EXTERNAL_TOOLCHAIN}/${ELT_TARGET_SYS}/libc/usr/include/${ELT_TARGET_SYS} ]; then
		cp -a ${EXTERNAL_TOOLCHAIN}/${ELT_TARGET_SYS}/libc/usr/include/${ELT_TARGET_SYS}/*  ${D}${includedir}
		rm -r ${D}${includedir}/${ELT_TARGET_SYS}
	fi

	# fix up the copied symlinks (they are still pointing to the multiarch directory)
	linker_name="${@bb.utils.contains("TUNE_FEATURES", "aarch64", "ld-linux-aarch64.so.1", bb.utils.contains("TUNE_FEATURES", "callconvention-hard", "ld-linux-armhf.so.3", "ld-linux.so.3",d), d)}"
	ln -sf ld-${ELT_VER_LIBC}.so ${D}${base_libdir}/${linker_name}
	ln -sf ../../lib/libnsl.so.1 ${D}${libdir}/libnsl.so
	ln -sf ../../lib/librt.so.1 ${D}${libdir}/librt.so
	ln -sf ../../lib/libcrypt.so.1 ${D}${libdir}/libcrypt.so
	ln -sf ../../lib/libnss_nis.so.2 ${D}${libdir}/libnss_nis.so
	ln -sf ../../lib/libresolv.so.2 ${D}${libdir}/libresolv.so
	ln -sf ../../lib/libnss_dns.so.2 ${D}${libdir}/libnss_dns.so
	ln -sf ../../lib/libnss_hesiod.so.2 ${D}${libdir}/libnss_hesiod.so
	ln -sf ../../lib/libutil.so.1 ${D}${libdir}/libutil.so
	ln -sf ../../lib/libnss_files.so.2 ${D}${libdir}/libnss_files.so
	ln -sf ../../lib/libnss_compat.so.2 ${D}${libdir}/libnss_compat.so
	ln -sf ../../lib/libcidn.so.1 ${D}${libdir}/libcidn.so
	ln -sf ../../lib/libBrokenLocale.so.1 ${D}${libdir}/libBrokenLocale.so
	ln -sf ../../lib/libthread_db.so.1 ${D}${libdir}/libthread_db.so
	ln -sf ../../lib/libthread_db.so.1 ${D}${libdir}/libthread_db-1.0.so
	ln -sf ../../lib/libanl.so.1 ${D}${libdir}/libanl.so
	ln -sf ../../lib/libdl.so.2 ${D}${libdir}/libdl.so
	ln -sf ../../lib/libnss_nisplus.so.2 ${D}${libdir}/libnss_nisplus.so
	ln -sf ../../lib/libnss_db.so.2 ${D}${libdir}/libnss_db.so
	ln -sf ../../lib/libm.so.6 ${D}${libdir}/libm.so
	ln -sf ../../lib/libatomic.so.1 ${D}${libdir}/libatomic.so
	ln -sf ../../lib/libgomp.so.1 ${D}${libdir}/libgomp.so
	ln -sf ../../lib/libitm.so.1 ${D}${libdir}/libitm.so
	ln -sf ../../lib/libssp.so.0 ${D}${libdir}/libssp.so
	ln -sf ../../lib/libstdc++.so.6 ${D}${libdir}/libstdc++.so
	ln -sf ../../lib/libubsan.so.0 ${D}${libdir}/libubsan.so
	if [ -f ${D}${base_libdir}/libasan.so.2 ]; then
		ln -sf ../../lib/libasan.so.2 ${D}${libdir}/libasan.so
	else
		ln -sf ../../lib/libasan.so.1 ${D}${libdir}/libasan.so
	fi
	if [ -f ${D}${base_libdir}/libgfortran.so.6 ]; then
		ln -sf ../../lib/libgfortran.so.6 ${D}${libdir}/libgfortran.so
	else
		ln -sf ../../lib/libgfortran.so.3 ${D}${libdir}/libgfortran.so
	fi

	# remove potential .so duplicates from base_libdir
	# for all symlinks created above in libdir
	rm -f ${D}${base_libdir}/libnsl.so
	rm -f ${D}${base_libdir}/librt.so
	rm -f ${D}${base_libdir}/libcrypt.so
	rm -f ${D}${base_libdir}/libnss_nis.so
	rm -f ${D}${base_libdir}/libresolv.so
	rm -f ${D}${base_libdir}/libnss_dns.so
	rm -f ${D}${base_libdir}/libnss_hesiod.so
	rm -f ${D}${base_libdir}/libutil.so
	rm -f ${D}${base_libdir}/libnss_files.so
	rm -f ${D}${base_libdir}/libnss_compat.so
	rm -f ${D}${base_libdir}/libcidn.so
	rm -f ${D}${base_libdir}/libBrokenLocale.so
	rm -f ${D}${base_libdir}/libthread_db.so
	rm -f ${D}${base_libdir}/libanl.so
	rm -f ${D}${base_libdir}/libdl.so
	rm -f ${D}${base_libdir}/libnss_nisplus.so
	rm -f ${D}${base_libdir}/libnss_db.so
	rm -f ${D}${base_libdir}/libm.so
	rm -f ${D}${base_libdir}/libasan.so
	rm -f ${D}${base_libdir}/libatomic.so
	rm -f ${D}${base_libdir}/libgomp.so
	rm -f ${D}${base_libdir}/libitm.so
	rm -f ${D}${base_libdir}/libssp.so
	rm -f ${D}${base_libdir}/libstdc++.so
	rm -f ${D}${base_libdir}/libgfortran.so
	rm -f ${D}${base_libdir}/libubsan.so

	# Besides ld-${ELT_VER_LIBC}.so, other libs can have duplicates like lib*-${ELT_VER_LIBC}.so
	# Only remove them if both are regular files and are identical
	for i in ${D}${base_libdir}/lib*-${ELT_VER_LIBC}.so; do
		f=$(echo $i | sed 's/-${ELT_VER_LIBC}//')
		l=$(ls $f.*)
		if [ $(readlink -f $i ) = $l ]; then
			echo "$i is a symlink of $l, keep it"
		elif [ $(readlink -f $l ) = $i ]; then
			echo "$l is a symlink of $i, keep it"
		else
			cmp -s $i $l
			if [ $? -eq 0 ]; then
				echo "$i is a duplicate of $l, remove it"
				rm $i
			else
				echo "$i and $l are different files, keep them both"
			fi
		fi
	done

	if [ -d ${D}${base_libdir}/arm-linux-gnueabi ]; then
	   rm -rf ${D}${base_libdir}/arm-linux-gnueabi
	fi

	if [ -d ${D}${base_libdir}/ldscripts ]; then
	   rm -rf ${D}${base_libdir}/ldscripts
	fi

	if [ -f ${D}${libdir}/libc.so ];then
		sed -i -e "s# /lib/${ELT_TARGET_SYS}# ../../lib#g" -e "s# /usr/lib/${ELT_TARGET_SYS}# .#g" -e "s# /lib/ld-linux# ../../lib/ld-linux#g" ${D}${libdir}/libc.so
	fi
	if [ -f ${D}${base_libdir}/libc.so ];then
		sed -i -e "s# /lib/${ELT_TARGET_SYS}# ../../lib#g" -e "s# /usr/lib/${ELT_TARGET_SYS}# .#g" ${D}${base_libdir}/libc.so
		if [ -f ${D}${base_libdir}/libc.so.6 ]; then
			sed -i -e "s# /usr/lib/libc.so.6# /lib/libc.so.6#g" ${D}${base_libdir}/libc.so
		fi
	fi
	if [ -f ${D}${libdir}/libpthread.so ];then
		sed -i -e "s# /lib/${ELT_TARGET_SYS}# ../../lib#g" -e "s# /usr/lib/${ELT_TARGET_SYS}# .#g" ${D}${libdir}/libpthread.so
	fi
	if [ -f ${D}${base_libdir}/libpthread.so ];then
		sed -i -e "s# /lib/${ELT_TARGET_SYS}# ../../lib#g" -e "s# /usr/lib/${ELT_TARGET_SYS}# .#g" ${D}${base_libdir}/libpthread.so
		if [ -f ${D}${base_libdir}/libpthread.so.0 ]; then
			sed -i -e "s# /usr/lib/libpthread.so.0# /lib/libpthread.so.0#g" ${D}${base_libdir}/libpthread.so
		fi
	fi

	# Remove if empty
	rmdir ${D}${bindir} || true
	rmdir ${D}${sbindir} || true
}

# PACKAGES is split up according to the 'source' recipes/includes in OE-core
# Stylistic differences are kept to make copy/pasting easier.

# From gcc-runtime.inc

PACKAGES =+ "\
    libstdc++ \
    libstdc++-precompile-dev \
    libstdc++-dbg \
    libstdc++-dev \
    libstdc++-staticdev \
    libg2c \
    libg2c-dev \
    libssp \
    libssp-dev \
    libssp-staticdev \
    libmudflap \
    libmudflap-dev \
    libmudflap-staticdev \
    libquadmath \
    libquadmath-dev \
    libquadmath-staticdev \
    libgomp \
    libgomp-dev \
    libgomp-staticdev \
    libatomic \
    libatomic-dev \
    libatomic-staticdev \
    libitm \
    libitm-dev \
    libitm-staticdev \
   "

# From gcc-sanitizers.inc

PACKAGES += "libasan libubsan liblsan libtsan"
PACKAGES += "libasan-dev libubsan-dev liblsan-dev libtsan-dev"
PACKAGES += "libasan-staticdev libubsan-staticdev liblsan-staticdev libtsan-staticdev"

# From libgfortran.inc:

PACKAGES += "\
    libgfortran \
    libgfortran-dev \
    libgfortran-staticdev \
   "

# libgcc.inc uses ${PN}, so replace that

PACKAGES += "\
    libgcc \
    libgcc-dev \
    libgcc-dbg \
"

# ... and the leftovers

PACKAGES =+ "\
        ${PN}-mtrace \
	libgcov-dev \
	linux-libc-headers \
	linux-libc-headers-dev \
"

INSANE_SKIP_${PN}-dbg = "staticdev"
INSANE_SKIP_${PN}-utils += "ldflags"
INSANE_SKIP_libstdc++ += "ldflags"
INSANE_SKIP_libgfortran += "ldflags"
INSANE_SKIP_libgcc += "ldflags"
INSANE_SKIP_libatomic += "ldflags"
INSANE_SKIP_libasan += "ldflags"
INSANE_SKIP_libubsan += "ldflags"
INSANE_SKIP_libssp += "ldflags"
INSANE_SKIP_libgomp += "ldflags"
INSANE_SKIP_libitm += "ldflags"
INSANE_SKIP_gdbserver += "ldflags"

# OE-core has literally listed 'glibc' in LIBC_DEPENDENCIES :/
RPROVIDES_${PN} = "glibc"
# Add runtime provides for the other libc* packages as well
RPROVIDES_${PN}-dev = "glibc-dev"
RPROVIDES_${PN}-doc = "glibc-doc"
RPROVIDES_${PN}-dbg = "glibc-dbg"
RPROVIDES_${PN}-pic = "glibc-pic"
RPROVIDES_${PN}-utils = "glibc-utils"
RPROVIDES_${PN}-mtrace = "glibc-mtrace libc-mtrace"

PKG_${PN} = "glibc"
PKG_${PN}-dev = "glibc-dev"
PKG_${PN}-doc = "glibc-doc"
PKG_${PN}-dbg = "glibc-dbg"
PKG_${PN}-pic = "glibc-pic"
PKG_${PN}-utils = "glibc-utils"
PKG_${PN}-mtrace = "glibc-mtrace"
PKG_${PN}-gconv = "glibc-gconv"
PKG_${PN}-extra-nss = "glibc-extra-nss"
PKG_${PN}-thread-db = "glibc-thread-db"
PKG_${PN}-pcprofile = "glibc-pcprofile"
PKG_${PN}-staticdev = "glibc-staticdev"

PKGV_${PN} = "${ELT_VER_LIBC}"
PKGV_${PN}-dev = "${ELT_VER_LIBC}"
PKGV_${PN}-doc = "${ELT_VER_LIBC}"
PKGV_${PN}-dbg = "${ELT_VER_LIBC}"
PKGV_${PN}-pic = "${ELT_VER_LIBC}"
PKGV_${PN}-utils = "${ELT_VER_LIBC}"
PKGV_${PN}-mtrace = "${ELT_VER_LIBC}"
PKGV_${PN}-gconv = "${ELT_VER_LIBC}"
PKGV_${PN}-extra-nss = "${ELT_VER_LIBC}"
PKGV_${PN}-thread-db = "${ELT_VER_LIBC}"
PKGV_${PN}-pcprofile = "${ELT_VER_LIBC}"
PKGV_${PN}-staticdev = "${ELT_VER_LIBC}"
PKGV_catchsegv = "${ELT_VER_LIBC}"
PKGV_glibc-extra-nss = "${ELT_VER_LIBC}"
PKGV_glibc-thread-db = "${ELT_VER_LIBC}"
PKGV_libcidn = "${ELT_VER_LIBC}"
PKGV_libmemusage = "${ELT_VER_LIBC}"
PKGV_libsegfault = "${ELT_VER_LIBC}"
PKGV_libsotruss = "${ELT_VER_LIBC}"
PKGV_sln = "${ELT_VER_LIBC}"
PKGV_nscd = "${ELT_VER_LIBC}"
PKGV_ldd = "${ELT_VER_LIBC}"

PKGV_libasan-dev = "${ELT_VER_GCC}"
PKGV_libasan = "${ELT_VER_GCC}"
PKGV_libasan-staticdev = "${ELT_VER_GCC}"
PKGV_libatomic-dev = "${ELT_VER_GCC}"
PKGV_libatomic = "${ELT_VER_GCC}"
PKGV_libatomic-staticdev = "${ELT_VER_GCC}"
PKGV_libg2c-dev = "${ELT_VER_GCC}"
PKGV_libg2c = "${ELT_VER_GCC}"
PKGV_libgcc-dev = "${ELT_VER_GCC}"
PKGV_libgcc = "${ELT_VER_GCC}"
PKGV_libgfortran-dev = "${ELT_VER_GCC}"
PKGV_libgfortran = "${ELT_VER_GCC}"
PKGV_libgfortran-staticdev = "${ELT_VER_GCC}"
PKGV_libgomp-dev = "${ELT_VER_GCC}"
PKGV_libgomp = "${ELT_VER_GCC}"
PKGV_libgomp-staticdev = "${ELT_VER_GCC}"
PKGV_libitm-dev = "${ELT_VER_GCC}"
PKGV_libitm = "${ELT_VER_GCC}"
PKGV_libitm-staticdev = "${ELT_VER_GCC}"
PKGV_liblsan-dev = "${ELT_VER_GCC}"
PKGV_liblsan = "${ELT_VER_GCC}"
PKGV_liblsan-staticdev = "${ELT_VER_GCC}"
PKGV_libmudflap-dev = "${ELT_VER_GCC}"
PKGV_libmudflap = "${ELT_VER_GCC}"
PKGV_libmudflap-staticdev = "${ELT_VER_GCC}"
PKGV_libquadmath-dev = "${ELT_VER_GCC}"
PKGV_libquadmath = "${ELT_VER_GCC}"
PKGV_libquadmath-staticdev = "${ELT_VER_GCC}"
PKGV_libssp-dev = "${ELT_VER_GCC}"
PKGV_libssp = "${ELT_VER_GCC}"
PKGV_libssp-staticdev = "${ELT_VER_GCC}"
PKGV_libstdc++-dbg = "${ELT_VER_GCC}"
PKGV_libstdc++-dev = "${ELT_VER_GCC}"
PKGV_libstdc++ = "${ELT_VER_GCC}"
PKGV_libstdc++-precompile-dev = "${ELT_VER_GCC}"
PKGV_libstdc++-staticdev = "${ELT_VER_GCC}"
PKGV_libtsan-dev = "${ELT_VER_GCC}"
PKGV_libtsan = "${ELT_VER_GCC}"
PKGV_libtsan-staticdev = "${ELT_VER_GCC}"
PKGV_libubsan-dev = "${ELT_VER_GCC}"
PKGV_libubsan = "${ELT_VER_GCC}"
PKGV_libubsan-staticdev = "${ELT_VER_GCC}"

PKGV_linux-libc-headers-dev = "${ELT_VER_KERNEL}"
PKGV_linux-libc-headers = "${ELT_VER_KERNEL}"

PKGV_gdbserver = "${ELT_VER_GDBSERVER}"

ALLOW_EMPTY_${PN}-mtrace = "1"
FILES_${PN}-mtrace = "${bindir}/mtrace"

FILES_libgcov-dev = "${libdir}/${TARGET_SYS}/${BINV}/libgcov.a"

FILES_libsegfault = "${base_libdir}/libSegFault*"

FILES_catchsegv = "${bindir}/catchsegv"
RDEPENDS_catchsegv = "libsegfault"

FILES_libquadmath = "${libdir}/libquadmath*.so.*"
SUMMARY_libquadmath = "GNU quad-precision math library"
FILES_libquadmath-dev = "\
    ${base_libdir}/gcc/${TARGET_SYS}/${BINV}/include/quadmath* \
    ${base_libdir}/libquadmath*.so \
    ${base_libdir}/libquadmath.la \
"
SUMMARY_libquadmath-dev = "GNU quad-precision math library - development files"
FILES_libquadmath-staticdev = "${base_libdir}/libquadmath.a"
SUMMARY_libquadmath-staticdev = "GNU quad-precision math library - static development files"

FILES_libatomic = "${base_libdir}/libatomic.so.*"
FILES_libatomic-dev = "\
    ${base_libdir}/libatomic.so \
    ${base_libdir}/libatomic.la \
"
FILES_libatomic-staticdev = "${base_libdir}/libatomic.a"

FILES_libasan = "${base_libdir}/libasan.so.*"
FILES_libasan-dev = "\
    ${base_libdir}/libasan.so \
    ${base_libdir}/libasan.la \
    ${base_libdir}/libsanitizer.spec \
"
FILES_libasan-staticdev = "${base_libdir}/libasan.a"

FILES_libubsan = "${base_libdir}/libubsan.so.*"
FILES_libubsan-dev = "\
    ${base_libdir}/libubsan.so \
    ${base_libdir}/libubsan.la \
"
FILES_libubsan-staticdev = "${base_libdir}/libubsan.a"

FILES_liblsan = "${base_libdir}/liblsan.so.*"
FILES_liblsan-dev = "\
    ${base_libdir}/liblsan.so \
    ${base_libdir}/liblsan.la \
"
FILES_libtsan-staticdev = "${base_libdir}/libtsan.a"

FILES_libtsan = "${base_libdir}/libtsan.so.*"
FILES_libtsan-dev = "\
    ${base_libdir}/libtsan.so \
    ${base_libdir}/libtsan.la \
"
FILES_libtsan-staticdev = "${base_libdir}/libtsan.a"

FILES_libgcc = "${base_libdir}/libgcc_s.so.1"
FILES_libgcc-dev = "${base_libdir}/libgcc_s.so"

FILES_linux-libc-headers = "\
	${includedir}/asm* \
	${includedir}/linux \
	${includedir}/mtd \
	${includedir}/rdma \
	${includedir}/scsi \
	${includedir}/sound \
	${includedir}/video \
"
FILES_${PN} += "\
	${libdir}/bin \
	${libdir}/locale \
	${libdir}/gconv/gconv-modules \
	${datadir}/zoneinfo \
	${base_libdir}/libcrypt*.so.* \
	${base_libdir}/libcrypt-*.so \
	${base_libdir}/libc.so.* \
	${base_libdir}/libc-*.so \
	${base_libdir}/libm.so.* \
	${base_libdir}/libmemusage.so \
	${base_libdir}/libm-*.so \
	${base_libdir}/ld*.so.* \
	${base_libdir}/ld-*.so \
	${base_libdir}/libpthread*.so.* \
	${base_libdir}/libpthread-*.so \
	${base_libdir}/libresolv*.so.* \
	${base_libdir}/libresolv-*.so \
	${base_libdir}/librt*.so.* \
	${base_libdir}/librt-*.so \
	${base_libdir}/libutil*.so.* \
	${base_libdir}/libutil-*.so \
	${base_libdir}/libnsl*.so.* \
	${base_libdir}/libnsl-*.so \
	${base_libdir}/libnss_files*.so.* \
	${base_libdir}/libnss_files-*.so \
	${base_libdir}/libnss_compat*.so.* \
	${base_libdir}/libnss_compat-*.so \
	${base_libdir}/libnss_dns*.so.* \
	${base_libdir}/libnss_dns-*.so \
	${base_libdir}/libnss_nis*.so.* \
	${base_libdir}/libnss_nisplus-*.so \
	${base_libdir}/libnss_nisplus*.so.* \
	${base_libdir}/libnss_nis-*.so \
	${base_libdir}/libnss_hesiod*.so.* \
	${base_libdir}/libnss_hesiod-*.so \
	${base_libdir}/libdl*.so.* \
	${base_libdir}/libdl-*.so \
	${base_libdir}/libanl*.so.* \
	${base_libdir}/libanl-*.so \
	${base_libdir}/libBrokenLocale*.so.* \
	${base_libdir}/libBrokenLocale-*.so \
	${base_libdir}/libcidn*.so.* \
	${base_libdir}/libcidn-*.so \
	${base_libdir}/libthread_db*.so.* \
	${base_libdir}/libthread_db-*.so \
	${base_libdir}/libmemusage.so \
	${base_libdir}/libSegFault.so \
	${base_libdir}/libpcprofile.so \
    "

FILES_${PN}-dbg += "${base_libdir}/debug"

FILES_libstdc++ = "${base_libdir}/libstdc++.so.*"
FILES_libstdc++-dev = "\
  ${includedir}/c++/ \
  ${base_libdir}/libstdc++.so \
  ${base_libdir}/libstdc++.la \
  ${base_libdir}/libsupc++.la"
FILES_libstdc++-staticdev = "\
  ${base_libdir}/libstdc++.a \
  ${base_libdir}/libsupc++.a"
FILES_libstdc++-dbg = "\
  ${base_libdir}/debug/libstdc++.*"

FILES_libstdc++-precompile-dev = "${includedir}/c++/${TARGET_SYS}/bits/*.gch"

FILES_libssp = "${base_libdir}/libssp.so.*"
FILES_libssp-dev = " \
  ${base_libdir}/libssp*.so \
  ${base_libdir}/libssp*_nonshared.a \
  ${base_libdir}/libssp*.la \
  ${base_libdir}/gcc/${TARGET_SYS}/${BINV}/include/ssp"
FILES_libssp-staticdev = " \
  ${base_libdir}/libssp*.a"

FILES_libgfortran = "${base_libdir}/libgfortran.so.*"
FILES_libgfortran-dev = " \
  ${base_libdir}/libgfortran.so \
  ${base_libdir}/libgfortran.spec"
FILES_libgfortran-staticdev = " \
  ${base_libdir}/libgfortran.a \
  ${base_libdir}/libgfortranbegin.a"

FILES_libmudflap = "${base_libdir}/libmudflap*.so.*"
FILES_libmudflap-dev = "\
  ${base_libdir}/libmudflap*.so \
  ${base_libdir}/libmudflap*.a \
  ${base_libdir}/libmudflap*.la"

FILES_libitm = "${base_libdir}/libitm*${SOLIBS}"
FILES_libitm-dev = "\
  ${base_libdir}/libitm*${SOLIBSDEV} \
  ${base_libdir}/libitm*.la \
  ${base_libdir}/libitm.spec \
  "
FILES_libitm-staticdev = "\
  ${base_libdir}/libitm*.a \
  "

FILES_libgomp = "${base_libdir}/libgomp*${SOLIBS}"
FILES_libgomp-dev = "\
  ${base_libdir}/libgomp*${SOLIBSDEV} \
  ${base_libdir}/libgomp*.la \
  ${base_libdir}/libgomp.spec \
  ${base_libdir}/gcc/${TARGET_SYS}/${BINV}/include/omp.h \
  "
FILES_libgomp-staticdev = "\
  ${base_libdir}/libgomp*.a \
  "
ELT_VER_MAIN ??= ""

python () {
    if not d.getVar("ELT_VER_MAIN", False):
        raise bb.parse.SkipPackage("External Linaro toolchain not configured (ELT_VER_MAIN not set).")
    import re
    notglibc = (re.match('.*uclibc$', d.getVar('TARGET_OS', True)) != None) or (re.match('.*musl$', d.getVar('TARGET_OS', True)) != None)
    if notglibc:
        raise bb.parse.SkipPackage("incompatible with target %s" %
                                   d.getVar('TARGET_OS', True))
}
