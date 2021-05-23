#ifndef __DECK_H_
#define __DECK_H_

#include <deque>
#include "Card.h"

namespace BlackJack
{

	class Deck
	{
	public:
		Deck();
		Card Draw();
		void Shuffle();
		void Collect();
		std::deque<Card> GetCards();
	private:
		std::deque<Card> cards;
	};

}

#endif
