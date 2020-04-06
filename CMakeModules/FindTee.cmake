
# FindTee
# --------
#
# Find tee
#
# This module looks for tee.  This module defines the following values:
#
# ::
#
#   TEE_EXECUTABLE: the full path to the tee tool.
#   TEE_FOUND: True if tee has been found.

find_program(TEE_EXECUTABLE tee)
mark_as_advanced( TEE_EXECUTABLE )

include (FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Tee FOUND_VAR REQUIRED_VARS TEE_EXECUTABLE)
if(Tee_FIND_REQUIRED AND NOT TEE_FOUND)
  message(FATAL_ERROR "Could not find tee")
endif()
