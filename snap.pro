# =============================================================================
#
# Snap - A Trick post processor with reports + plotting
#
# =============================================================================

CONFIG += ordered
TEMPLATE = subdirs
SUBDIRS = libopts \
          libqplot \
          libsnapdata \
          libsnaprt \
          monte \
          snap \
          snapx \
          snapq \
          testsnap

OTHER_FILES += \
    snap.supp \
    monte.supp

monte.depends += snaprt
monte.depends += snapdata
monte.depends += opts
monte.depends += qplot

snap.depends += snaprt
snap.depends += snapdata
snap.depends += opts
snap.depends += qplot

snapx.depends += snaprt
snapx.depends += snapdata
snapx.depends += opts
snapx.depends += qplot

snapq.depends += snaprt
snapq.depends += snapdata
snapq.depends += opts
snapq.depends += qplot

testsnap.depends += snaprt
testsnap.depends += snapdata
testsnap.depends += opts
testsnap.depends += qplot
