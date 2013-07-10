# Findlibxml2.cmake
# --
# Find the libxml2 library
#
# This module defines:
#   libxml2_INCLUDE_DIRS - where to find libxml/xpath.h
#   libxml2_LIBRARIES    - xml2
#   libxml2_FOUND        - True if libxml2 was found

Include(FindModule)
FIND_MODULE(libxml2 libxml/xpath.h "" "libxml2" xml2 "" "")
