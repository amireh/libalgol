CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

IF(DEFINED CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ELSE()
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ENDIF()

INCLUDE (CheckIncludeFileCXX)
INCLUDE (CMakeDependentOption)

PROJECT(algol)

LIST(APPEND CMAKE_MODULE_PATH
            ${CMAKE_SOURCE_DIR}/CMake/Scripts
            ${CMAKE_SOURCE_DIR}/CMake/Modules )

ADD_DEFINITIONS("-Wno-deprecated -Wall -Wextra -Wno-reorder -std=c++0x")

OPTION(ALGOL_LOG_FQN "Log entries will be prefixed with the fully-qualified name of the application" OFF)
OPTION(ALGOL_PYTHON_BINDINGS "Build Python SWIG bindings" OFF)
OPTION(ALGOL_LUA_ENGINE "Build the Lua engine" OFF)
OPTION(ALGOL_LUA_BINDINGS "Build Lua SWIG bindings" OFF)
OPTION(ALGOL_MESSAGING "Build the AMQP messaging components" OFF)
OPTION(ALGOL_ANALYTICS "Build the AMQP analytics components (requires ALGOL_MESSAGING)" OFF)
OPTION(ALGOL_ADMIN "Build the algolSH shell and admin console components" OFF)
OPTION(ALGOL_TESTS "Build C++ tests" OFF)

FIND_PACKAGE(Boost 1.46 COMPONENTS filesystem thread date_time system REQUIRED)
FIND_PACKAGE(log4cpp REQUIRED)
FIND_PACKAGE(YAJL REQUIRED)
FIND_PACKAGE(CURL REQUIRED)
FIND_PACKAGE(PCRECPP REQUIRED)
FIND_PACKAGE(libxml2 REQUIRED)

# project version
SET( ALGOL_VERSION_MAJOR 0 )
SET( ALGOL_VERSION_MINOR 8 )
SET( ALGOL_VERSION_PATCH 3 )
SET( ALGOL_VERSION_BUILD "a" )
SET( ALGOL_VERSION "${ALGOL_VERSION_MAJOR}.${ALGOL_VERSION_MINOR}.${ALGOL_VERSION_PATCH}-${ALGOL_VERSION_BUILD}")

INCLUDE_DIRECTORIES(include
                    ${Boost_INCLUDE_DIRS}
                    ${CURL_INCLUDE_DIR}
                    ${log4cpp_INCLUDE_DIR}
                    ${YAJL_INCLUDE_DIR}
                    ${PCRECPP_INCLUDE_DIR}
                    ${libxml2_INCLUDE_DIR}
                    .
                    )

# The linking is done for tests
LINK_LIBRARIES( ${Boost_LIBRARIES}
                ${log4cpp_LIBRARIES}
                ${YAJL_LIBRARIES}
                ${CURL_LIBRARIES}
                ${PCRECPP_LIBRARIES}
                ${libxml2_LIBRARIES}
                pthread)

IF (ALGOL_MESSAGING)
  FIND_PACKAGE(RabbitMQ REQUIRED)
  FIND_PACKAGE(BSON REQUIRED)

  INCLUDE_DIRECTORIES( ${RabbitMQ_INCLUDE_DIR} ${BSON_INCLUDE_DIR})
  LINK_LIBRARIES( ${RabbitMQ_LIBRARIES} ${BSON_LIBRARIES} )

ENDIF()

IF (ALGOL_LUA_BINDINGS OR ALGOL_LUA_ENGINE)
  FIND_PACKAGE(Lua 5.1 REQUIRED)
  INCLUDE_DIRECTORIES( ${Lua_INCLUDE_DIR_51} )
  LINK_LIBRARIES( ${Lua_LIBRARY_51} )
ENDIF()

IF (ALGOL_PYTHON_BINDINGS)
	FIND_PACKAGE(PythonLibs REQUIRED)
ENDIF()

SET(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib")
SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin")

CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/algol_config.hpp.cmake ${CMAKE_SOURCE_DIR}/include/algol/algol_config.hpp )

ADD_SUBDIRECTORY(src)

IF (ALGOL_TESTS)
  ADD_SUBDIRECTORY(test/unit)
ENDIF()

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/algol DESTINATION include)
FILE(GLOB cmake_mods "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules/*.cmake")
FILE(GLOB cmake_scripts "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Scripts/*.cmake")
INSTALL(FILES libalgol-config.cmake DESTINATION lib/${PROJECT_NAME})
INSTALL(FILES ${cmake_mods} DESTINATION lib/${PROJECT_NAME})
INSTALL(FILES ${cmake_scripts} DESTINATION lib/${PROJECT_NAME})
