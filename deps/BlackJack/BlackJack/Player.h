#ifndef __PLAYER_H_
#define __PLAYER_H_

#include <vector>
#include <string>

#include "Hand.h"
#include "Card.h"

namespace BlackJack
{

	class Player
	{
	public:
		Player(std::string name);
		bool WillHit();
		bool IsBusted();
		void TakeCard(Card&);
		void SetAnte(uint32_t);
		uint8_t GetHandValue();
		void AddCash(uint32_t);
		std::string GetName();
		Hand GetHand();
		void PrintHand();
		uint32_t GetAnte();
		uint32_t GetCash();
		void DiscardHand();
		bool operator==(const Player& player);

		bool HasCurrentTurn;
		std::string Name;
		Hand hand;
		uint32_t Ante;
		uint32_t Cash;
	};
}

#endif
