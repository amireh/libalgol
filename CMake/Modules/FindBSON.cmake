# FindBSON.cmake
# --
# Find the BSON-cpp library
#
# This module defines:
#   BSON_INCLUDE_DIRS - where to find bson/bson.h
#   BSON_LIBRARIES    - bson-cpp
#   BSON_FOUND        - True if BSON-cpp was found

Include(FindModule)
FIND_MODULE(BSON bson/bson.h "" "" bson-cpp "" "")
