# CMakeList.txt : CMake project for KaliDiscordBot4, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(BUILD_SHARED_LIBS TRUE)
set(HEADERS
	deps/BlackJack/src/Card.h
)
target_link_libraries(KaliDiscordBot4 deps/BlackJack/include/BlackJack.dll)

# Add source to this project's executable.
add_executable (KaliDiscordBot4 "KaliDiscordBot4.cpp" "KaliDiscordBot4.h")

# TODO: Add tests and install targets if needed.
