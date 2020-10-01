cmake_minimum_required (VERSION 3.6)
project ("KaliDiscordBot4")

add_executable(KaliDiscordBot4.exe KaliDiscordBot4/KaliDiscordBot4.cpp)

#include_directories(deps/sleepy-discord/include)
#include_directories(deps/sleepy-discord/deps/websocketpp)

add_subdirectory(deps/sleepy-discord)
target_link_libraries(KaliDiscordBot4.exe sleepy-discord)

