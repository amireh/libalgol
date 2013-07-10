SET(WRAPPERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/wrappers)
SET(WRAPPER "${WRAPPERS_DIR}/lua_algol_wrap.cxx")

# generate Python bindings
ADD_CUSTOM_COMMAND( OUTPUT ${WRAPPER} COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/generate_wrappers.py ${CMAKE_CURRENT_SOURCE_DIR} "Generating Lua bindings")

ADD_DEFINITIONS("-std=c++0x -w")

# the bindings library
ADD_LIBRARY(lua_algol SHARED ${WRAPPER})
SET_TARGET_PROPERTIES(lua_algol PROPERTIES PREFIX "")
SET_SOURCE_FILES_PROPERTIES( ${WRAPPER} PROPERTIES GENERATED 1)

TARGET_LINK_LIBRARIES(lua_algol algol)
# TARGET_LINK_LIBRARIES(lua_algol supc++ boost_system boost_thread boost_regex boost_filesystem log4cpp algol)

ADD_CUSTOM_TARGET(lua_algol_bindings DEPENDS ${WRAPPER})

IF(NOT EXISTS "${WRAPPER}")
  MESSAGE(STATUS "Lua bindings are not yet generated, forcing generation. Generate manually using make lua_algol_bindings")
  ADD_DEPENDENCIES(lua_algol lua_algol_bindings)
ENDIF()


INSTALL(TARGETS lua_algol LIBRARY DESTINATION lib)
