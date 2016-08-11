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
SUBDIRS = libsnap \
          snapq \
          testsnap
