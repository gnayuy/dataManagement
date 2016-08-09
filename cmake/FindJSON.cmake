# - Find JSON
#
# Find the CGAL includes and libraries
# This module defines
#  JSON_FOUND           - TRUE if found, FALSE otherwise
#  JSON_INCLUDE_DIR     - Include directories for JSON
#  JSON_LIBRARIES       - The libraries to link against to use JSON

FIND_PACKAGE(PackageHandleStandardArgs)

SET(JSON_SEARCHPATH
    /usr/include/
    /usr/local/include/
    /opt/include/
    /opt/local/include/
    )

FIND_PATH(JSON_INCLUDE_DIR
    NAMES json/json.h
    PATHS ${JSON_SEARCHPATH}
    DOC "The JSON include directory")

FIND_LIBRARY(JSON_LIBRARIES
    NAMES json libjson jsoncpp libjsoncpp
    PATHS
    /usr/lib
    /usr/local/lib
    /usr/lib64
    /usr/local/lib64
    /opt/lib
    /opt/local/lib
    /opt/lib64
    /opt/local/lib64
    DOC "The JSON libraries")

IF(JSON_INCLUDE_DIR AND JSON_LIBRARIES)
    SET(JSON_FOUND TRUE)
ELSE(JSON_INCLUDE_DIR AND JSON_LIBRARIES)
    SET(JSON_FOUND FALSE)
ENDIF(JSON_INCLUDE_DIR AND JSON_LIBRARIES)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(JSON REQUIRED_VARS JSON_INCLUDE_DIR JSON_LIBRARIES)

MARK_AS_ADVANCED( JSON_INCLUDE_DIR JSON_LIBRARIES )
