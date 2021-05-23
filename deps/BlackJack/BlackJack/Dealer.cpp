#include <iostream>

#include "Dealer.h"
#include "Game.h"


namespace BlackJack
{
	Dealer::Dealer()
		: game(nullptr),
		  Player("Dealer"), 
		  players(std::vector<Player>()), 
		  deck(Deck()), 
		  pot(500000)
	{
	}

	Dealer::Dealer(const Game* const game)
		: game(game), 
		  Player("Dealer"), 
		  players(std::vector<Player>()), 
		  deck(Deck()), 
		  pot(500000)
	{
	}

	const Dealer& Dealer::operator=(const Dealer& dealer)
	{
		return dealer;
	}

	Deck Dealer::GetDeck()
	{
		return deck;
	}

	void Dealer::AddPlayer(Player& player)
	{
		players.push_back(player);
	}

	void Dealer::RemovePlayer(Player& player)
	{
		Player* playerToRemove = nullptr;
		for (Player& p : players)
		{
			if (p.Name == player.Name)
			{
				playerToRemove = &p;
			}
		}
		if (playerToRemove != nullptr)
		{
			std::remove(players.begin(), players.end(), *playerToRemove);
		}
	}

	bool Dealer::AllPlayersAreBusted()
	{
		for (Player& player : players)
			if (!player.IsBusted())
				return false;
		return true;
	}

	void Dealer::PlayTurn(Player& player)
	{
		player.HasCurrentTurn = true;
		std::cout << player.GetName() << std::endl;
		player.PrintHand();
		while (player.WillHit())
		{
			DealHit(player);
			player.PrintHand();
			if (player.IsBusted())
			{
				std::cout << player.GetName() << " busts" << std::endl;
				break;
			}
		}
		player.HasCurrentTurn = false;
	}

	void Dealer::PlayTurn()
	{
		std::cout << Name << std::endl;
		PrintHand();
		while (ShouldHit())
		{
			DealHit(*this);
			PrintHand();
			if (IsBusted())
			{
				std::cout << Name << " busts" << std::endl;
				break;
			}
		}
	}

	void Dealer::GetAntes()
	{
		for (Player& player : players)
		{
			uint32_t ante = 0;
			while (ante < this->game->nMinAnte)
			{
				std::string input;
				std::cout << "Ante: ";
				std::cin >> input;
				ante = std::strtol(input.c_str(), 0, 10);
			}
			player.SetAnte(ante);
		}
	}

	void Dealer::PlayGame()
	{
		DealHands();
		std::cout << "Dealer" << std::endl;
		Hand.FlipOne();
		for (Player& player : players)
			PlayTurn(player);

		if (!AllPlayersAreBusted())
			PlayTurn();
		EndGame();
	};

	void Dealer::EndGame()
	{
		CollectOrDistributeAntes();
		for (Player& player : players)
		{
			player.DiscardHand();
			player.SetAnte(0);
		}
		DiscardHand();
		if (Hand.GetCards().size() <= DECK_SIZE/2)
			deck.Collect();
	};

	bool Dealer::ShouldHit()
	{
		return Hand.GetDealerValue() <= 16;
	};

	void Dealer::DealHands()
	{
		for (Player& player : players)
		{
			DealHit(player);
			DealHit(player);
		}

		DealHit(*this);
		DealHit(*this);
	};

	Card Dealer::DealHit(Player& player)
	{
		Card card = deck.Draw();
		player.TakeCard(card);
		return card;
	};

	bool Dealer::WinsAgainst(Player& player)
	{
		return (!IsBusted() && (player.IsBusted() || player.GetHandValue() < Hand.GetDealerValue()));
	}

	bool Dealer::LosesAgainst(Player& player)
	{
		return (IsBusted() || (!player.IsBusted() && player.GetHandValue() > Hand.GetDealerValue()));
	}

	void Dealer::CollectOrDistributeAntes()
	{
		for (Player& player : players)
		{
			if (WinsAgainst(player))
			{
				CollectFrom(player);
			}
			else if (LosesAgainst(player))
			{
				DistributeTo(player);
			}
			// else draw
		}
	};

	void Dealer::CollectFrom(Player& player)
	{
		std::cout << Name << " collects " << player.GetAnte() << " from " << player.GetName() << std::endl;
		pot += player.GetAnte();
	}

	void Dealer::DistributeTo(Player& player)
	{
		if (player.GetAnte() <= pot)
		{
			std::cout << player.GetName() << " collects " << player.GetAnte() << " from " << Name << std::endl;
			pot -= player.GetAnte();
			player.AddCash(player.GetAnte());
		}
	}

}

