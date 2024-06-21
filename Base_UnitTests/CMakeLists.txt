cmake_minimum_required(VERSION 3.22)

project(caffaBase_UnitTests)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(PROJECT_FILES cafBase_UnitTests.cpp cafUuidGeneratorTests.cpp)

# add the executable
add_executable(${PROJECT_NAME} ${PROJECT_FILES})

source_group("" FILES ${PROJECT_FILES})

find_package(GTest CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} caffaBase GTest::gtest ${THREAD_LIBRARY})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
