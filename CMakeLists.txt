cmake_minimum_required(VERSION 3.16)
project(seka_cli LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SEKA_WITH_SERVER "Build WebSocket server (requires Boost)" ON)
if (SEKA_WITH_SERVER)
    find_package(Boost COMPONENTS system)
endif()

add_library(seka_core
    core/card.cpp
    core/deck.cpp
    core/hand.cpp
    core/evaluator.cpp
    core/secure_rng.cpp
    core/shuffle.cpp
    core/sha256.cpp
)

target_include_directories(seka_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(seka_cli_lib
    src/card.cpp
    src/deck.cpp
    src/hand.cpp
    src/rules.cpp
    src/game.cpp
    src/ui.cpp
)

target_include_directories(seka_cli_lib PUBLIC include)

add_library(seka_server_logic
    server/room_manager.cpp
    server/tournament_manager.cpp
    server/table_logic.cpp
    server/random_util.cpp
)

target_include_directories(seka_server_logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(seka_server_logic PRIVATE seka_core)

add_executable(seka_cli src/main.cpp)
target_link_libraries(seka_cli PRIVATE seka_cli_lib)

if (SEKA_WITH_SERVER AND Boost_FOUND)
    add_executable(seka_server
        server/main.cpp
        server/server.cpp
        server/session.cpp
        server/server_state.cpp
        server/room_manager.cpp
        server/tournament_manager.cpp
        server/table_logic.cpp
        server/random_util.cpp
        server/protocol.cpp
    )

    target_include_directories(seka_server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(seka_server PRIVATE seka_core)
    target_link_libraries(seka_server PRIVATE Boost::system)
else()
    message(WARNING "Boost not found; seka_server target will not be built.")
endif()

add_executable(seka_tests tests/test_rules.cpp)
target_link_libraries(seka_tests PRIVATE seka_core)

add_executable(seka_shuffle_tests tests/test_shuffle.cpp)
target_link_libraries(seka_shuffle_tests PRIVATE seka_core)

add_executable(seka_tournament_tests tests/test_tournament.cpp)
target_link_libraries(seka_tournament_tests PRIVATE seka_core seka_server_logic)
