DESCRIPTION = "The Battle for Wesnoth is a turn-based strategy game with a fantasy theme."
HOMEPAGE = "http://www.wesnoth.org/"
SECTION = "games"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://copyright;md5=3cd35059318048afea9baa2fb45e339b"

DEPENDS = "asio freetype libsdl-image libsdl-mixer libsdl-net libsdl-ttf zlib boost pango libpng"

SHRT_VER = "${@oe.utils.trim_version("${PV}", 2)}"

SRC_URI = " \
    ${SOURCEFORGE_MIRROR}/${PN}/${PN}-${SHRT_VER}/${P}/${P}.tar.bz2 \
    file://0001-Find-sdl-CFLAGS-with-pkg-config-sdl-config-is-not-us.patch \
"

ARM_INSTRUCTION_SET = "arm"

inherit cmake pkgconfig

EXTRA_OECMAKE = "\
	-DENABLE_EDITOR=ON \
	-DENABLE_LOW_MEM=ON \
	-DENABLE_FRIBIDI=OFF \
	\
	-DCMAKE_BUILD_TYPE=Debug \
	-DENABLE_STRICT_COMPILATION=OFF \
	"

PACKAGES = "${PN}-editor ${PN}-doc ${PN}-music ${PN}-sounds \
	${PN}-aoi ${PN}-did ${PN}-ei ${PN}-httt ${PN}-l \
	${PN}-nr ${PN}-sof ${PN}-sotbe ${PN}-thot ${PN}-trow \
	${PN}-tsg ${PN}-tb ${PN}-utbs ${PN}-low\
	${PN}-data \
	${PN}-all-campaigns \
	${PN}-all \
	${PN} ${PN}-dbg \
	${PN}-server \
	"

DESCRIPTION_${PN}-editor = "Map Editor for The Battle for ${PN}"
DESCRIPTION_${PN}-all = "The Battle for ${PN} with all campaigns, music and sounds"
DESCRIPTION_${PN}-all-campaigns = "The Battle for ${PN} with all campaigns."
DESCRIPTION_${PN}-sounds = "Optional sound package for The Battle for ${PN}"
DESCRIPTION_${PN}-music = "Optional music package for The Battle for ${PN}"
DESCRIPTION_${PN}-data = "Mandatory data package for The Battle for ${PN}"
DESCRIPTION_${PN}-server = "Optional Battle for Wesnoth server"

ALLOW_EMPTY_${PN}-all-campaigns = "1"

do_configure_prepend(){
	export HOST_SYS="${HOST_SYS}"
	export BUILD_SYS="${BUILD_SYS}"
	export STAGING_LIBDIR="${STAGING_LIBDIR}"
	export STAGING_INCDIR="${STAGING_INCDIR}"
	rm -f ${S}/cmake/FindBoost.cmake
}

do_install_append() {
	#ugly hack but otherwise it would have required to
	#have MANDIR:STRING=share/man that would require a
	#second python function
	if [ -d ${D}${prefix}/man ];then
		mv ${D}${prefix}/man ${D}${mandir}
	fi

        rm -Rf ${D}/run
        rm -Rf ${D}/var
}

RDEPENDS_${PN} = "${PN}-data tremor"

RDEPENDS_${PN}-data = "bash python"

RDEPENDS_${PN}-editor = "${PN}-data"

RDEPENDS_${PN}-all-campaigns = "${PN} \
	${PN}-aoi ${PN}-did ${PN}-ei ${PN}-httt ${PN}-l \
	${PN}-nr ${PN}-sof ${PN}-sotbe ${PN}-thot ${PN}-trow \
	${PN}-tsg ${PN}-tb ${PN}-utbs ${PN}-low"

# Installing wesnoth-all should pull everything in (like in Debian).
RDEPENDS_${PN}-all = "${PN} ${PN}-sounds ${PN}-music"

FILES_${PN}-music = "\
	${datadir}/wesnoth/data/core/music \
"

FILES_${PN}-sounds = "\
	${datadir}/wesnoth/data/core/sounds \
"

# Picks up remaining translations and data. Must be packaged after
# wesnoth-music, wesnoth-sounds and all campaigns.
FILES_${PN}-data = "\
	${datadir}/wesnoth/sounds \
	${datadir}/wesnoth/images \
	${datadir}/wesnoth/data \
	${datadir}/wesnoth/fonts \
	${datadir}/wesnoth/translations \
	${datadir}/wesnoth/l10n-track \
"

FILES_${PN} = "\
	${bindir}/wesnoth \
	${datadir}/icons \
	${datadir}/applications/wesnoth.desktop \
	${datadir}/pixmaps/wesnoth-icon.png \
"

FILES_${PN}-server = "\
	${bindir}/wesnothd \
	${localstatedir}/run/wesnothd \
"

FILES_${PN}-low ="\
       ${datadir}/wesnoth/data/campaigns/Legend_of_Wesmere \
       ${datadir}/wesnoth/translations/*/LC_MESSAGES/wesnoth-aoi.mo \
"

FILES_${PN}-editor = "\
	${bindir}/wesnoth_editor \
	${datadir}/applications/wesnoth_editor.desktop \
	${datadir}/pixmaps/wesnoth_editor-icon.png \
"

FILES_${PN}-aoi = "\
	${datadir}/wesnoth/data/campaigns/An_Orcish_Incursion \
	${datadir}/wesnoth/translations/*/LC_MESSAGES/wesnoth-aoi.mo \
"

FILES_${PN}-did = "\
	${datadir}/wesnoth/data/campaigns/Descent_Into_Darkness \
	${datadir}/wesnoth/translations/*/LC_MESSAGES/wesnoth-did.mo \
"

FILES_${PN}-ei = "\
	${datadir}/wesnoth/data/campaigns/Eastern_Invasion \
	${datadir}/wesnoth/translations/*/LC_MESSAGES/wesnoth-ei.mo \
"

FILES_${PN}-httt = "\
	${datadir}/wesnoth/data/campaigns/Heir_To_The_Throne \
	${datadir}/wesnoth/translations/*/LC_MESSAGES/wesnoth-httt.mo \
"

FILES_${PN}-l = "\
	${datadir}/wesnoth/data/campaigns/Liberty \
	${datadir}/wesnoth/translations/*/LC_MESSAGES/wesnoth-l.mo \
"

FILES_${PN}-nr = "\
	${datadir}/wesnoth/data/campaigns/Northern_Rebirth \
	${datadir}/wesnoth/translations/*/LC_MESSAGES/wesnoth-nr.mo \
"

FILES_${PN}-sof = "\
	${datadir}/wesnoth/data/campaigns/Sceptre_of_Fire \
	${datadir}/wesnoth/translations/*/LC_MESSAGES/wesnoth-sof.mo \
"

FILES_${PN}-sotbe = "\
	${datadir}/wesnoth/data/campaigns/Son_Of_The_Black_Eye \
	${datadir}/wesnoth/translations/*/LC_MESSAGES/wesnoth-sotbe.mo \
"

FILES_${PN}-thot = "\
	${datadir}/wesnoth/data/campaigns/The_Hammer_of_Thursagan \
	${datadir}/wesnoth/translations/*/LC_MESSAGES/wesnoth-thot.mo \
"

FILES_${PN}-trow = "\
	${datadir}/wesnoth/data/campaigns/The_Rise_Of_Wesnoth \
	${datadir}/wesnoth/translations/*/LC_MESSAGES/wesnoth-trow.mo \
"

FILES_${PN}-tsg = "\
	${datadir}/wesnoth/data/campaigns/The_South_Guard \
	${datadir}/wesnoth/translations/*/LC_MESSAGES/wesnoth-tsg.mo \
"

FILES_${PN}-tb = "\
	${datadir}/wesnoth/data/campaigns/Two_Brothers \
	${datadir}/wesnoth/translations/*/LC_MESSAGES/wesnoth-tb.mo \
"

FILES_${PN}-utbs = "\
	${datadir}/wesnoth/data/campaigns/Under_the_Burning_Suns \
	${datadir}/wesnoth/translations/*/LC_MESSAGES/wesnoth-utbs.mo \
"

SRC_URI[md5sum] = "3f460a494530d32aa5d5d0f19c95efbd"
SRC_URI[sha256sum] = "8a3b5a3409a57b646536e547094e1bb8bcd87797e00cd63184152222e377253e"
