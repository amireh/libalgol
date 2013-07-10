# add sources
SET(algol_SRCS

  ../include/algol/algol.hpp
  ../include/algol/configurable.hpp
  ../include/algol/exception.hpp
  ../include/algol/log.hpp
  ../include/algol/file_manager.hpp
  ../include/algol/regex.hpp
  ../include/algol/platform.hpp
  ../include/algol/monitor.hpp
  ../include/algol/logger.hpp
  ../include/algol/log_manager.hpp
  ../include/algol/utility.hpp
  ../include/algol/configurator.hpp
  ../include/algol/identifiable.hpp
  ../include/algol/log4cpp/file_log_layout.hpp
  ../include/algol/log4cpp/vanilla_log_layout.hpp
  ../include/algol/log4cpp/syslog_log_layout.hpp
  ../include/algol/binreloc/binreloc.h

  algol.cpp
  log_manager.cpp
  logger.cpp
  file_manager.cpp
  monitor.cpp
  regex.cpp
  configurable.cpp
  configurator.cpp
  identifiable.cpp
  utility.cpp

  md5.cpp

  plugin_manager.cpp
  dyn_lib.cpp

  http/request.cpp
  http/reply.cpp

  log4cpp/file_log_layout.cpp
  log4cpp/syslog_log_layout.cpp
  log4cpp/vanilla_log_layout.cpp

  binreloc/binreloc.c
)

IF(ALGOL_MESSAGING)
  LIST(APPEND algol_SRCS
              messaging/station.cpp
              messaging/channel.cpp
              messaging/channel_consumer.cpp
              messaging/communicator.cpp
              messaging/message.cpp)


  IF (ALGOL_ANALYTICS)
    LIST(APPEND algol_SRCS
                analytics/tracker.cpp
                analytics/sheet.cpp)
  ENDIF()
ENDIF()

IF(ALGOL_ADMIN)
  LIST(APPEND algol_SRCS
              ../include/algol/admin/console.hpp
              ../include/algol/admin/bot.hpp
              ../include/algol/admin/connection.hpp
              ../include/algol/admin/dispatcher.hpp
              ../include/algol/admin/message.hpp

              admin/console.cpp
              admin/bot.cpp
              admin/connection.cpp
              admin/dispatcher.cpp
              admin/message.cpp)

  ADD_SUBDIRECTORY(algolSH)
ENDIF()

IF(ALGOL_LUA_ENGINE)
  LIST(APPEND algol_SRCS
              ../include/algol/lua/engine.hpp

              lua/engine.cpp)
ENDIF()

ADD_DEFINITIONS("-DENABLE_BINRELOC")
ADD_DEFINITIONS("-Wno-deprecated -std=c++0x")

IF(ALGOL_PYTHON_BINDINGS)
  ADD_SUBDIRECTORY(bindings/python/SWIG)
ENDIF()

IF(ALGOL_LUA_BINDINGS)
  ADD_SUBDIRECTORY(bindings/lua/SWIG)
ENDIF()

# the executable
ADD_LIBRARY(algol SHARED ${algol_SRCS})
#~ ADD_LIBRARY(algol_static STATIC ${algol_SRCS})

SET_TARGET_PROPERTIES(algol PROPERTIES VERSION ${ALGOL_VERSION})
#~ SET_TARGET_PROPERTIES(algol_static PROPERTIES VERSION ${ALGOL_VERSION})
#~ SET_TARGET_PROPERTIES(algol_static PROPERTIES OUTPUT_NAME algol)

INSTALL(TARGETS algol LIBRARY DESTINATION lib)
#~ INSTALL(TARGETS algol_static ARCHIVE DESTINATION lib)

MESSAGE(STATUS "Version: " ${ALGOL_VERSION})
