cmake_minimum_required(VERSION 3.16)

if(NOT VERSION)
  set(VERSION 1.5.0)
endif()

project(
  caffaBase
  VERSION ${VERSION}
  DESCRIPTION "Caffa Base Utility Library")

message(STATUS "Building ${PROJECT_NAME} version ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-${GIT_HASH}")

set(CMAKE_CXX_STANDARD 20)

option(CAFFA_BUILD_UNIT_TESTS "Build unit tests" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(PUBLIC_HEADERS cafAssert.h cafLogger.h cafStringTools.h cafNotNull.h cafUuidGenerator.h)
set(PROJECT_FILES cafAssert.cpp cafLogger.cpp cafStringTools.cpp cafUuidGenerator.cpp)

if(CAFFA_BUILD_SHARED)
  message(STATUS "Building ${PROJECT_NAME} shared")
  add_library(${PROJECT_NAME} SHARED ${PROJECT_FILES})
else()
  add_library(${PROJECT_NAME} STATIC ${PROJECT_FILES})
endif()

add_definitions(-DGTEST_HAS_STD_WSTRING=0)
add_definitions(-DSPDLOG_NO_ATOMIC_LEVELS)
set(UUID_USING_CXX20_SPAN
    ON
    CACHE INTERNAL "Use CXX20 span")
set(UUID_BUILD_TESTS
    OFF
    CACHE INTERNAL "Build stduuid tests")

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${CMAKE_PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${CMAKE_PROJECT_VERSION_MAJOR})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
  target_compile_definitions(${PROJECT_NAME} PRIVATE _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
  set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/W4 /wd4100 /wd4127")
  set_target_properties(${PROJECT_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

if(CAFFA_BUILD_UNIT_TESTS)
  add_subdirectory(Base_UnitTests)
  enable_testing()
  add_test(caffaBase_UnitTests ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/caffaBase_UnitTests)
endif()

target_link_libraries(${PROJECT_NAME})

install(
  TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

source_group("" FILES ${PROJECT_FILES})
