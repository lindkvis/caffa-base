cmake_minimum_required(VERSION 3.16)

project(caffaBase_UnitTests)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(PROJECT_FILES cafBase_UnitTests.cpp cafUuidGeneratorTests.cpp)

find_package(Boost 1.74.0 REQUIRED COMPONENTS regex)
find_package(GTest REQUIRED)

# add the executable
add_executable(${PROJECT_NAME} ${PROJECT_FILES})

source_group("" FILES ${PROJECT_FILES})

target_link_libraries(${PROJECT_NAME} caffaBase GTest::gtest ${THREAD_LIBRARY} Boost::regex)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
