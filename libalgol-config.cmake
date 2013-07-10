# Locate algol Shared library
#
# This module defines:
#  libalgol_FOUND, if false, do not try to link to algol
#  libalgol_LIBRARY
#  libalgol_INCLUDE_DIRS, where to find algol/algol.hpp

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR} )

SET(Boost_USE_SHARED_LIBS ON)
SET(Boost_USE_STATIC_LIBS OFF)

FIND_PACKAGE(Boost 1.46 COMPONENTS filesystem thread date_time system regex REQUIRED)
FIND_PACKAGE(log4cpp REQUIRED)
FIND_PACKAGE(YAJL REQUIRED)
FIND_PACKAGE(libxml2 REQUIRED)
FIND_PACKAGE(CURL REQUIRED)
FIND_PACKAGE(PCRECPP REQUIRED)
FIND_PACKAGE(BSON)
FIND_PACKAGE(RabbitMQ)
FIND_PACKAGE(Lua 5.1)

SET(libalgol_PATHS
  /usr
  /usr/local
  /usr/local/libalgol
  ../libalgol
  /home/kandie/Workspace/Projects/algol/libalgol
)

FIND_PATH(libalgol_INCLUDE_DIR algol/algol.hpp
  HINTS
  PATH_SUFFIXES include
  PATHS
  ${libalgol_PATHS}
)

FIND_LIBRARY(libalgol_LIBRARY
  NAMES algol
  HINTS
  PATH_SUFFIXES lib
  PATHS
  ${libalgol_PATHS}
)

IF(libalgol_LIBRARY AND libalgol_INCLUDE_DIR)

  SET(libalgol_INCLUDE_DIRS
      ${libalgol_INCLUDE_DIR}
      ${Boost_INCLUDE_DIRS}
      ${log4cpp_INCLUDE_DIRS}
      ${YAJL_INCLUDE_DIRS}
      ${libxml2_INCLUDE_DIRS}
      ${CURL_INCLUDE_DIRS}
      ${PCRECPP_INCLUDE_DIRS})

  SET( libalgol_LIBRARIES
    ${libalgol_LIBRARY}
    ${Boost_LIBRARIES}
    ${YAJL_LIBRARIES}
    ${log4cpp_LIBRARIES}
    ${libxml2_LIBRARIES}
    ${CURL_LIBRARIES}
    ${PCRECPP_LIBRARIES}
    pthread)

  IF(BSON_FOUND)
    LIST(APPEND libalgol_INCLUDE_DIRS ${BSON_INCLUDE_DIRS})
    LIST(APPEND libalgol_LIBRARIES ${BSON_LIBRARIES})
  ENDIF()
  
  IF(RabbitMQ_FOUND)
    LIST(APPEND libalgol_INCLUDE_DIRS ${RabbitMQ_INCLUDE_DIRS})
    LIST(APPEND libalgol_LIBRARIES ${RabbitMQ_LIBRARIES})
  ENDIF()

  IF(Lua_FOUND)
    LIST(APPEND libalgol_INCLUDE_DIRS ${Lua_INCLUDE_DIR_51})
    LIST(APPEND libalgol_LIBRARIES ${Lua_LIBRARY_51})
  ENDIF()

  SET( libalgol_FOUND TRUE )
  MESSAGE(STATUS "Found algol library")
ELSE()
  MESSAGE(STATUS "Error! Could not find algol library")
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(libalgol         DEFAULT_MSG
                                  libalgol_LIBRARY libalgol_INCLUDE_DIR)

MARK_AS_ADVANCED(libalgol_INCLUDE_DIR libalgol_LIBRARY)
