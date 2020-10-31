SUMMARY = "CacaoVM for use as OpenEmbedded's Java VM"
HOMEPAGE = "http://www.cacaojvm.org/"
LICENSE  = "GPL-2.0"
LIC_FILES_CHKSUM = "file://COPYING;md5=59530bdf33659b29e73d4adb9f9f6552"
SECTION  = "interpreters"

PROVIDES = "virtual/java-initial-native"

inherit native autotools-brokensep

DEPENDS = "zlib-native libtool-native fastjar-native classpath-initial-native jikes-initial-native"

SRC_URI = "\
        http://www.complang.tuwien.ac.at/cacaojvm/download/cacao-${PV}/cacao-${PV}.tar.bz2; \
        file://cacao-0.98-initial.patch \
        file://disable_hw_exceptions.patch \
        file://cacao-0.9.8-initial-fix-gc-undefined-behaviour.patch \
        file://cacao-0.9.8-do-not-rely-on-absolute-paths.patch \
        "

# libjvm disabled - it would conflict with cacao-native installations
EXTRA_OECONF = "\
    --enable-debug \
    --with-classpath-libdir=${libdir} \
    --with-classpath-includedir=${includedir}/classpath-initial \
    --with-classpath-classes=${datadir}/classpath-initial/glibj.zip \
    --with-vm-zip=${datadir}/cacao-initial/vm.zip \
    --program-suffix=-initial \
    --disable-libjvm \
    "

# enforces the usage of jikes-initial
export JAVAC="jikes-initial"

# enforces the usage of fastjar
export JAR="fastjar"

do_configure_append() {
  # Fix the executable name in the wrapper script.
  sed -i -e "s|exec cacao \\$|exec cacao-initial \\$|" src/scripts/java.in
}

SRC_URI[md5sum] = "8b8907c8b925761c9410bcadb9705346"
SRC_URI[sha256sum] = "cb9363add825cedf77764fc49a223aaf43f0a9f485b711ba8c92f16b13fff188"
