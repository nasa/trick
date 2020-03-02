# =============================================================================
#
# koviz - Fast plotting for Trick simulation data
#
# =============================================================================

PROJECT_NAME = koviz
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

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    QT += printsupport
}

exists( /usr/include/mpv/client.h ) {
    DEFINES += HAS_MPV
    LIBS += -lmpv
}

#CONFIG += debug
