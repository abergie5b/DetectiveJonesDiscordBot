#ifndef __GAME_H_
#define __GAME_H_

#include "Dealer.h"

namespace BlackJack
{
	class Game
	{
		public:
			uint32_t nMinAnte;
			uint8_t nPlayers;
			Dealer dealer;
			bool IsPlaying;

			Game();
			Game(Player& player);
			Game(uint8_t nPlayers, uint32_t nMinAnte);

			void Play();
			void SetOptions();
			std::vector<Player>& GetPlayers();
			void AddPlayer(Player& player);
			void RemovePlayer(Player& player);
			void SetMinAnte(uint32_t min);
			Player* GetPlayer(const std::string& name);
	};

}

#endif
