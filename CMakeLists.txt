CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

PROJECT(
  ntrusive
  VERSION 0.1.0
  LANGUAGES CXX
  DESCRIPTION "Header-only intrusive doubly-linked list"
  HOMEPAGE_URL "https://github.com/niko256/ntrusive"
)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# *---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---* #

SET(NTRUSIVE_IS_ROOT_PROJECT OFF)
IF (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  SET(NTRUSIVE_IS_ROOT_PROJECT ON)
ENDIF()

OPTION(NTRUSIVE_BUILD_TESTS
  "Build unit tests"
  ${NTRUSIVE_IS_ROOT_PROJECT}
)

# *---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---* #

OPTION(INTRUSIVE_LIST_ENABLE_ASAN "Enable AddressSanitizer" OFF)
OPTION(INTRUSIVE_LIST_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

# *---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---* #

ADD_LIBRARY(ntrusive INTERFACE)

ADD_LIBRARY(ntrusive::ntrusive ALIAS ntrusive)


TARGET_INCLUDE_DIRECTORIES(
  ntrusive
  INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

TARGET_COMPILE_FEATURES(
  ntrusive
  INTERFACE
  cxx_std_20
)

# *---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---* #

SET(NTRUSIVE_HEADERS
  include/ntrusive/base_node.hpp
  include/ntrusive/intrusive.hpp
  include/ntrusive/iterator.hpp
  include/ntrusive/list.hpp
  include/ntrusive/node.hpp
)

TARGET_SOURCES(
  ntrusive
  INTERFACE
  ${NTRUSIVE_HEADERS}
)

# *---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---* #

IF(NTRUSIVE_BUILD_TESTS)
  enable_testing()
  ADD_SUBDIRECTORY(tests)
ENDIF()

# *---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---* #
