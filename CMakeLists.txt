cmake_minimum_required (VERSION 3.6)
project ("KaliDiscordBot4")


add_executable(KaliDiscordBot4.exe KaliDiscordBot4/helpers.cpp KaliDiscordBot4/KaliCommand.cpp KaliDiscordBot4/KaliDiscordBot4.cpp KaliDiscordBot4/BlackJackClient.cpp)

add_subdirectory(deps/BlackJack)
set_target_properties(BlackJack PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(deps/sleepy-discord)
target_link_libraries(KaliDiscordBot4.exe sleepy-discord BlackJack)
