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
          testsnap

OTHER_FILES += \
    testsnap.supp
