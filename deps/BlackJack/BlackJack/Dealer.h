#ifndef __DEALER_H_
#define __DEALER_H_

#include <vector>

#include "Deck.h"
#include "Player.h"


namespace BlackJack
{
	class Game;
	class Dealer : public Player
	{
	public:
		Dealer();
		Dealer(const Game* const game);
		const Dealer& operator=(const Dealer& dealer);

		void PlayGame();
		void PlayTurn(Player& player);
		void AddPlayer(Player& player);
		void RemovePlayer(Player& player);

		Deck GetDeck();
		bool ShouldHit();
		bool WinsAgainst(Player& player);
		bool LosesAgainst(Player& player);
		Card DealHit(Player& player);
		void EndGame();
		void GetAntes();
		void PlayTurn();
		bool AllPlayersAreBusted();
		void DealHands();
		void CollectOrDistributeAntes();
		void CollectFrom(Player& player);
		void DistributeTo(Player& player);

		//
		std::vector<Player> players;
		Deck deck;
		uint32_t pot;
		const Game* const game;
	};

}

#endif

