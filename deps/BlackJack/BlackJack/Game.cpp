#include "Dealer.h"
#include "Game.h"

#include <iostream>

namespace BlackJack
{

    Game::Game(uint8_t nPlayers, uint32_t nMinAnte)
        : nPlayers(nPlayers),
          nMinAnte(nMinAnte),
          dealer(Dealer(this)),
		  IsPlaying(false)
    {
    }

    Game::Game()
    {
        this->nPlayers = 0;
        this->nMinAnte = 0;
        this->dealer = Dealer(this);
        this->IsPlaying = false;
    };

    Game::Game(Player& player)
        : nPlayers(0),
          nMinAnte(0),
          dealer(Dealer(this)),
          IsPlaying(false)
    {
        this->AddPlayer(player);
    };

    void Game::SetMinAnte(uint32_t min)
    {
        this->nMinAnte = min;
    }

	Player* Game::GetPlayer(const std::string& name)
    {
        for (Player& player : this->dealer.players)
        {
            if (player.Name == name)
                return &player;
        }
        return nullptr;
    }

    void Game::SetOptions()
    {
        std::string input;
        std::cout << "#Players: ";
        std::cin >> input;
        this->nPlayers = std::strtol(input.c_str(), 0, 10);

        for (int x = 0; x < nPlayers; x++)
        {
            Player player("Player");
            this->AddPlayer(player);
        }

        std::cout << "Min Ante: ";
        std::cin >> input;
        std::cout << std::endl;
        this->nMinAnte = std::strtol(input.c_str(), 0, 10);
    }

    void Game::Play()
    {
        std::string playAgain = "y";
        while (playAgain == "y")
        {
			this->dealer.GetAntes();

            this->IsPlaying = true;
            dealer.PlayGame();
            dealer.EndGame();
            this->IsPlaying = false;

            //
            std::cout << "Play Again?" << std::endl;
            std::cin >> playAgain;
        }
    };

    std::vector<Player>& Game::GetPlayers()
    {
        return this->dealer.players;
    }

    void Game::AddPlayer(Player& player)
    {
        this->dealer.AddPlayer(player);
        this->nPlayers++;
    };

    void Game::RemovePlayer(Player& player)
    {
        this->dealer.RemovePlayer(player);
    }

}
