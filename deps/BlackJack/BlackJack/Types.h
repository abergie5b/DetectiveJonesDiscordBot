#ifndef __TYPES_H_
#define __TYPES_H_

#include "Card.h"

#include <map>
#include <string>

namespace BlackJack
{
	const uint8_t BLACKJACK = 21;
	const uint8_t DECK_SIZE = 52;

	enum class CardNames
	{
		NONE,
		UNUSED,
		TWO,
		THREE,
		FOUR,
		FIVE,
		SIX,
		SEVEN,
		EIGHT,
		NINE,
		TEN,
		JACK,
		QUEEN,
		KING,
		ACE,
		LAST
	};
	const std::map<CardNames, std::string> Cards =
	{
		{ CardNames::TWO, "Two" },
		{ CardNames::THREE, "Three" },
		{ CardNames::FOUR, "Four" },
		{ CardNames::FIVE, "Five" },
		{ CardNames::SIX, "Six" },
		{ CardNames::SEVEN, "Seven" },
		{ CardNames::EIGHT, "Eight" },
		{ CardNames::NINE, "Nine" },
		{ CardNames::TEN, "Ten" },
		{ CardNames::JACK, "Jack" },
		{ CardNames::QUEEN, "Queen" },
		{ CardNames::KING, "King" },
		{ CardNames::ACE, "Ace" },
	};

	enum class SuiteNames
	{
		NONE,
		HEARTS,
		DIAMONDS,
		CLUBS,
		SPADES,
		LAST
	};
	const std::map<SuiteNames, std::string> Suites =
	{
		{ SuiteNames::HEARTS, "Hearts" },
		{ SuiteNames::DIAMONDS, "Diamonds" },
		{ SuiteNames::CLUBS, "Clubs" },
		{ SuiteNames::SPADES, "Spades" }
	};

}


#endif 
