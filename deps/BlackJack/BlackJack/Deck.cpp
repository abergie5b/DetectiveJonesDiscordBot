#include <random>
#include <time.h>
#include <algorithm>

#include "Deck.h"

namespace BlackJack
{

	Deck::Deck()
	{
		srand(time(0));
		Collect();
	}

	std::deque<Card> Deck::GetCards()
	{
		return cards;
	}

	Card Deck::Draw()
	{
		Card card;
		if (!cards.empty())
		{
			card = cards.front();
			cards.pop_front();
		}
		return card;
	}

	void Deck::Shuffle()
	{
		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(cards.begin(), cards.end(), g);
	};

	void Deck::Collect()
	{
		std::deque<Card> _cards;
		for (int x = 2; x<(int)CardNames::LAST; x++)
		{
			for (int y=1; y<(int)SuiteNames::LAST; y++)
			{
				_cards.push_back(Card(
					CardNames(x), SuiteNames(y))
				);
			}
		}
		cards = _cards;
		Shuffle();
	};

}

