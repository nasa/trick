
# FindMaven
# --------
#
# Find mvn
#
# This module looks for mvn.  This module defines the following values:
#
# ::
#
#   MAVEN_EXECUTABLE: the full path to the tee tool.
#   MAVEN_FOUND: True if tee has been found.

find_program(MAVEN_EXECUTABLE mvn)
include (FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MAVEN FOUND_VAR MAVEN_FOUND REQUIRED_VARS MAVEN_EXECUTABLE)
if(Maven_FIND_REQUIRED AND NOT MAVEN_FOUND)
  message(FATAL_ERROR "Could not find mvn")
endif()
