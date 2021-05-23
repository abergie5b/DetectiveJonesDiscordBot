#ifndef __HAND_H_
#define __HAND_H_

#include <vector>

#include "Card.h"

namespace BlackJack
{

	class Hand
	{
	public:
		std::vector<Card> GetCards();
		uint8_t GetValue();
		uint8_t GetDealerValue();
		bool IsBusted();
		void push_back(Card&);
		void Print();
		void Print(Card&);
		void FlipOne();
		void Discard();
	private:
		std::vector<Card> cards;
	};

}

#endif

