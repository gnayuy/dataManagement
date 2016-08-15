# - Find CPPREST
#
# Find the CPPREST includes and libraries
# This module defines
#  CPPREST_FOUND           - TRUE if found, FALSE otherwise
#  CPPREST_INCLUDE_DIR     - Include directories for CPPREST
#  CPPREST_LIBRARIES       - The libraries to link against to use CPPREST

FIND_PACKAGE(PackageHandleStandardArgs)

SET(CPPREST_SEARCHPATH
    /usr/include/
    /usr/local/include/
    /opt/include/
    /opt/local/include/
    )

FIND_PATH(CPPREST_INCLUDE_DIR
    NAMES cpprest/http_client.h
    PATHS ${CURL_SEARCHPATH}
    DOC "The CURL include directory")

FIND_LIBRARY(CPPREST_LIBRARIES
    NAMES cpprest libcpprest
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

IF(CPPREST_INCLUDE_DIR AND CPPREST_LIBRARIES)
    SET(CPPREST_FOUND TRUE)
ELSE(CPPREST_INCLUDE_DIR AND CPPREST_LIBRARIES)
    SET(CPPREST_FOUND FALSE)
ENDIF(CPPREST_INCLUDE_DIR AND CPPREST_LIBRARIES)

# boost
FIND_PACKAGE(Boost COMPONENTS chrono thread system random regex REQUIRED)
IF(Boost_FOUND)
    SET(BOOST_LIBRARIES ${Boost_LIBRARIES})
ENDIF(Boost_FOUND)

# openssl
FIND_PACKAGE(OPENSSL REQUIRED)
IF(OPENSSL_FOUND)
    SET(OPENSSL_LIBRARIES ${OPENSSL_LIBRARIES})
ENDIF(OPENSSL_FOUND)

SET(CPPREST_LIBRARIES ${CPPREST_LIBRARIES} ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CPPREST REQUIRED_VARS CPPREST_INCLUDE_DIR CPPREST_LIBRARIES)

MARK_AS_ADVANCED( CPPREST_INCLUDE_DIR CPPREST_LIBRARIES )
