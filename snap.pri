# =============================================================================
#
# Snap - Fast report quality plotting for Trick simulation data
#
# =============================================================================

PROJECT_NAME = snap
PROJECT_VERSION = 1.0.0

INSTALL_PREFIX = /usr/local
isEmpty(PREFIX) {
   message("Current install path is '$${INSTALL_PREFIX}'")
   message("To override the install path run: 'qmake PREFIX=/custom/path'")
   PREFIX = $$INSTALL_PREFIX
} else {
   INSTALL_PREFIX = $${PREFIX}
}
INSTALL_LIBDIR = $${PREFIX}/lib

#CONFIG += debug
