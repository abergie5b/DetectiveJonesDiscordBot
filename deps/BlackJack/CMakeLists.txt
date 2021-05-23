cmake_minimum_required (VERSION 3.6)
project(BlackJack)

set(MODULE_FILES
	BlackJack/Card.cpp 
	BlackJack/Card.h
	BlackJack/Dealer.cpp 
	BlackJack/Dealer.h
	BlackJack/Deck.cpp 
	BlackJack/Deck.h
	BlackJack/Game.cpp 
	BlackJack/Game.h
	BlackJack/Hand.cpp 
	BlackJack/Hand.h
	BlackJack/Player.cpp 
	BlackJack/Player.h
	BlackJack/Types.h
)

add_library(BlackJack STATIC ${MODULE_FILES})
target_include_directories(BlackJack PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS BlackJack DESTINATION BlackJack-d)
install(FILES Game.h DESTINATION BlackJack-include)

