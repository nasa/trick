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
SUBDIRS = libkplot \
          libqplot \
          snap \
          snapq \
          csv2trk \
          trk2csv \
          testsnap

OTHER_FILES += \
    snap.supp \
    monte.supp

snap.depends += qplot

snapq.depends += qplot
snapq.depends += kplot

testsnap.depends += qplot
testsnap.depends += kplot
