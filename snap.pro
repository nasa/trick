# =============================================================================
#
# Snap - A Trick post processor with reports + plotting
#
# =============================================================================

PROJECT_NAME = snap
PROJECT_VERSION = 1.0.0

INSTALL_PREFIX = /usr/local
isEmpty(PREFIX) {
   message("Current install path is '$${INSTALL_PREFIX}'")
   message("To override the install path run: 'qmake PREFIX=/custom/path'")
} else {
   INSTALL_PREFIX = $${PREFIX}
}
INSTALL_LIBDIR = $${INSTALL_PREFIX}/lib
	 

CONFIG += ordered
TEMPLATE = subdirs
SUBDIRS = libopts2 \
          libqplot \
          libsnapdata \
          libsnaprt \
          monte \
          snap \
          snapx \
          snapq \
          snapdiff \
          testsnap

OTHER_FILES += \
    snap.supp \
    monte.supp

monte.depends += snaprt
monte.depends += snapdata
monte.depends += opts2
monte.depends += qplot

snap.depends += snaprt
snap.depends += snapdata
snap.depends += opts2
snap.depends += qplot

snapx.depends += snaprt
snapx.depends += snapdata
snapx.depends += opts
snapx.depends += qplot

snapq.depends += snaprt
snapq.depends += snapdata
snapq.depends += opts2
snapq.depends += qplot

snapdiff.depends += snaprt
snapdiff.depends += snapdata
snapdiff.depends += opts2
snapdiff.depends += qplot

testsnap.depends += snaprt
testsnap.depends += snapdata
testsnap.depends += opts2
testsnap.depends += qplot
