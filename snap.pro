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
SUBDIRS = libopts \
          libqplot \
          libsnapdata \
          libsnaprt \
          snap \
          snapq \
          csv2trk \
          trk2csv \
          testsnap

OTHER_FILES += \
    snap.supp \
    monte.supp

snap.depends += snaprt
snap.depends += snapdata
snap.depends += opts
snap.depends += qplot

snapq.depends += snaprt
snapq.depends += snapdata
snapq.depends += opts
snapq.depends += qplot

testsnap.depends += snaprt
testsnap.depends += snapdata
testsnap.depends += opts
testsnap.depends += qplot
