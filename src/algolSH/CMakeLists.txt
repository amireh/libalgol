# add sources
SET(SRCS
  utility.cpp
  algolSH.cpp
  connection.cpp
  delegate.cpp
  main.cpp

)

ADD_DEFINITIONS("-std=c++0x -DALGOLSH")

# the executable
ADD_EXECUTABLE(algolsh ${SRCS})
TARGET_LINK_LIBRARIES(algolsh readline algol dl)

INSTALL(TARGETS algolsh DESTINATION bin)
