cmake_minimum_required(VERSION 3.14)
project(TicTacToeCourse VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT DEFINED BUILD_TTTCORE)
    set(BUILD_TTTCORE "SOURCE" CACHE STRING "Build tttcore from source or use prebuilt library")
    set_property(CACHE BUILD_TTTCORE PROPERTY STRINGS "SOURCE" "PREBUILT")
endif()

if(BUILD_TTTCORE STREQUAL "PREBUILT")
    add_library(tttcore STATIC IMPORTED)
    set_target_properties(tttcore PROPERTIES
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/libs/libtttcore.a"
    )
    target_include_directories(tttcore INTERFACE ${CMAKE_SOURCE_DIR}/src)
    message(STATUS "Using PREBUILT libtttcore.a from libs/")
else()
    set(TTT_CORE_SOURCES
        src/core/state.cpp
        src/core/game.cpp
        src/core/field.cpp
        src/core/event.cpp
    )
    add_library(tttcore STATIC ${TTT_CORE_SOURCES})
    target_include_directories(tttcore PUBLIC ${CMAKE_SOURCE_DIR}/src)
    message(STATUS "Building tttcore from SOURCE")
endif()

set(TTT_PLAYER_SOURCES
    src/player/my_observer.cpp
    src/player/my_player.cpp
)
add_library(tttplayer STATIC ${TTT_PLAYER_SOURCES})
target_include_directories(tttplayer PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(tttplayer PUBLIC tttcore)

add_executable(test_stats_vs_baseline tests/test_stats_vs_baseline.cpp)
target_include_directories(test_stats_vs_baseline PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_stats_vs_baseline PRIVATE tttplayer tttcore)

add_executable(test_my_player_vs_human tests/test_my_player_vs_human.cpp)
target_include_directories(test_my_player_vs_human PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_my_player_vs_human PRIVATE tttplayer tttcore)

message(STATUS "CMake configured successfully!")
message(STATUS "BUILD_TTTCORE: ${BUILD_TTTCORE}")
message(STATUS "Targets: tttcore, tttplayer, test_stats_vs_baseline, test_my_player_vs_human")