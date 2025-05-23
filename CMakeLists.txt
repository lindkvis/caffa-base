cmake_minimum_required(VERSION 3.16)

set(CAFFA_BASE_VERSION 1.7.1)

project(
        caffaBase
        VERSION ${CAFFA_BASE_VERSION}
        DESCRIPTION "Caffa Base Utility Library")

include(cmake/git_submodule.cmake)

message(STATUS "Building ${PROJECT_NAME} version ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-${GIT_HASH}")

set(CMAKE_CXX_STANDARD 20)

option(CAFFA_BUILD_UNIT_TESTS "Build unit tests" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(PUBLIC_HEADERS cafAssert.h cafLogger.h cafStringTools.h cafNotNull.h cafUuidGenerator.h)
set(PROJECT_FILES cafAssert.cpp cafLogger.cpp cafStringTools.cpp cafUuidGenerator.cpp)

find_package(Boost 1.74.0 REQUIRED COMPONENTS regex)

if (CAFFA_BUILD_SHARED)
    message(STATUS "Building ${PROJECT_NAME} shared")
    add_library(${PROJECT_NAME} SHARED ${PROJECT_FILES})
else ()
    add_library(${PROJECT_NAME} STATIC ${PROJECT_FILES})
endif ()

add_definitions(-DGTEST_HAS_STD_WSTRING=0)
add_definitions(-DSPDLOG_NO_ATOMIC_LEVELS)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Boost::regex)
if (MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/W4 /wd4100 /wd4127")
    set_target_properties(${PROJECT_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    target_link_libraries(${PROJECT_NAME} bcrypt.lib)
endif ()

if (CAFFA_BUILD_UNIT_TESTS)
    add_subdirectory(Base_UnitTests)
    enable_testing()
    add_test(caffaBase_UnitTests ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/caffaBase_UnitTests)
endif ()

install(
        TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

source_group("" FILES ${PROJECT_FILES})
