# - Try to find cURLpp
# Once done this will define
# CURLPP_FOUND - System has cURLpp
# CURLPP_INCLUDE_DIR - The cURLpp include directories
# CURLPP_LIBRARY - The libraries needed to used CURLPP
# CURLPP_VERSION - cURLpp version

include(LibFindMacros)

# Dependencies
libfind_package(cURLpp CURL)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(CURLPP curlpp)

# Include dir
find_path(CURLPP_INCLUDE_DIR
    NAMES curlpp
    PATHS ${CURLPP_INCLUDE_DIRS}
    )

# Finally the library itself
find_library(CURLPP_LIBRARY
    NAMES curlpp
    PATHS ${CURLPP_LIBRARY_DIRS}
    )

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(CURLPP_PROCESS_INCLUDES CURLPP_INCLUDE_DIR)
set(CURLPP_PROCESS_LIBS CURLPP_LIBRARY)
libfind_process(CURLPP)

# Extract cURLpp version
libfind_version_header(CURLPP curlpp/cURLpp.hpp LIBCURLPP_VERSION)

