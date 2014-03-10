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
