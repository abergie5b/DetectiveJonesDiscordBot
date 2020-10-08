cmake_minimum_required (VERSION 3.6)
project ("KaliDiscordBot4")

add_executable(KaliDiscordBot4.exe KaliDiscordBot4/helpers.cpp KaliDiscordBot4/KaliCommand.cpp KaliDiscordBot4/KaliDiscordBot4.cpp)

#include_directories(deps/sleepy-discord/include)
#include_directories(deps/sleepy-discord/deps/websocketpp)

#target_include_directories(KaliDiscordBot4.exe PRIVATE C:/Users/Andrew/Source/vcpkg/installed/x86-windows/include)

add_subdirectory(deps/sleepy-discord)
target_link_libraries(KaliDiscordBot4.exe sleepy-discord)

