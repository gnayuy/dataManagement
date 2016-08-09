# - Find CURL
#
# Find the CGAL includes and libraries
# This module defines
#  CURL_FOUND           - TRUE if found, FALSE otherwise
#  CURL_INCLUDE_DIR     - Include directories for CURL
#  CURL_LIBRARIES       - The libraries to link against to use CURL

FIND_PACKAGE(PackageHandleStandardArgs)

SET(CURL_SEARCHPATH
    /usr/include/
    /usr/local/include/
    /opt/include/
    /opt/local/include/
    )

FIND_PATH(CURL_INCLUDE_DIR
    NAMES curl/curl.h
    PATHS ${CURL_SEARCHPATH}
    DOC "The CURL include directory")

FIND_LIBRARY(CURL_LIBRARIES
    NAMES CURL libCURL
    PATHS
    /usr/lib
    /usr/local/lib
    /usr/lib64
    /usr/local/lib64
    /opt/lib
    /opt/local/lib
    /opt/lib64
    /opt/local/lib64
    DOC "The CURL libraries")

IF(CURL_INCLUDE_DIR AND CURL_LIBRARIES)
    SET(CURL_FOUND TRUE)
ELSE(CURL_INCLUDE_DIR AND CURL_LIBRARIES)
    SET(CURL_FOUND FALSE)
ENDIF(CURL_INCLUDE_DIR AND CURL_LIBRARIES)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CURL REQUIRED_VARS CURL_INCLUDE_DIR CURL_LIBRARIES)

MARK_AS_ADVANCED( CURL_INCLUDE_DIR CURL_LIBRARIES )
