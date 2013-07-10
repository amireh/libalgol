
# Since FindPythonInterp and FindPythonLibs are outdated and messy,
# we'll resort to hacks involving calling the python interpreter
# to retrieve the user favorite's python version (as algol_py works
# fine on Python 2 and 3) as well as the extensions directory (site-packages).

# Get the site-packages directory and store in PYTHON_SITE_PACKAGES
EXECUTE_PROCESS ( COMMAND python -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(True))"
                  OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)

# Python include dirs
EXECUTE_PROCESS ( COMMAND python-config --includes  OUTPUT_VARIABLE PYTHON_INCLUDE_DIRS OUTPUT_STRIP_TRAILING_WHITESPACE)

MESSAGE(STATUS "Python include dirs: ${PYTHON_INCLUDE_DIRS}")

SET(WRAPPERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/wrappers)
SET(WRAPPER "${WRAPPERS_DIR}/algol_py_wrap.cxx")

# generate Python bindings
ADD_CUSTOM_COMMAND( OUTPUT ${WRAPPER} COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/generate_wrappers.py ${CMAKE_CURRENT_SOURCE_DIR} "Generating Python bindings")

ADD_DEFINITIONS("-std=c++0x -w ${PYTHON_INCLUDE_DIRS}")

# the bindings library
ADD_LIBRARY(algol_py SHARED ${WRAPPER})
SET_TARGET_PROPERTIES(algol_py PROPERTIES PREFIX "_")
SET_SOURCE_FILES_PROPERTIES( ${WRAPPER} PROPERTIES GENERATED 1)

TARGET_LINK_LIBRARIES(algol_py supc++ boost_system boost_thread boost_regex boost_filesystem log4cpp algol)

ADD_CUSTOM_TARGET(algol_py_bindings DEPENDS ${WRAPPER})

IF(NOT EXISTS "${WRAPPER}")
  MESSAGE(STATUS "Python bindings are not yet generated, forcing generation. Generate manually using make algol_py_bindings")
  ADD_DEPENDENCIES(algol_py algol_py_bindings)
ENDIF()


INSTALL(FILES ${WRAPPERS_DIR}/algol_py.py DESTINATION ${PYTHON_SITE_PACKAGES})
# INSTALL(FILES ${WRAPPERS_DIR}/algol_py.pyc DESTINATION ${PYTHON_SITE_PACKAGES})
INSTALL(TARGETS algol_py DESTINATION ${PYTHON_SITE_PACKAGES})