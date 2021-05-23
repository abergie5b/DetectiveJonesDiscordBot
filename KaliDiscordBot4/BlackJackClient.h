#include <BlackJack/Card.h>
#include <BlackJack/Deck.h>
#include <BlackJack/Hand.h>
#include <BlackJack/Dealer.h>
#include <BlackJack/Game.h>
#include <BlackJack/Player.h>
#include <BlackJack/Types.h>

#include <sleepy_discord/snowflake.h>
#include <sleepy_discord/channel.h>
#include <sleepy_discord/sleepy_discord.h>

#include "helpers.h"

#include <map>

class BlackJackClient
{
public:
	static const std::string GAME_START_STR;
	static const std::string GAME_DOES_NOT_EXIST_STR;
	static const std::string GetCardStr(BlackJack::Player, BlackJack::Card);
	static const std::string GetDealerBustsStr();
	static const std::string GetPlayerBustsStr(BlackJack::Player player);
	static const std::string GetPlayerPassStr(BlackJack::Player player);
	static const std::string GetPlayerStartTurnStr(BlackJack::Player player);
	static const std::string GetCollectFromPlayerStr(BlackJack::Player player);
	static const std::string GetDistributeToPlayerStr(BlackJack::Player player);
	static const std::string GetRoundOverStr(BlackJack::Game* game);
	static const std::string GetHandStr(BlackJack::Player& player);
	static const std::string GetLeaveGameStr(BlackJack::Player& player);

	BlackJackClient();
	BlackJackClient(const BlackJackClient&) = default;
	BlackJackClient& operator=(const BlackJackClient&) = default;
	~BlackJackClient() = default;

	BlackJack::Player* GetNextPlayer(BlackJack::Game* game, BlackJack::Player* player);
	std::string CreateGame(std::string channelId, BlackJack::Player player);
	BlackJack::Game* GetGame(std::string channelId);
	std::string GetGameInfoStr(std::string channelId);
	bool AllPlayersHaveAnted(std::string channelId);
	void DealHands(std::string channelId);
	void EndGame(BlackJack::Game* game, SleepyDiscord::DiscordClient& client, SleepyDiscord::Snowflake<SleepyDiscord::Channel> channelId);
	void PlayDealerTurn(BlackJack::Game* game, SleepyDiscord::DiscordClient& client, SleepyDiscord::Snowflake<SleepyDiscord::Channel> channelId);
	void StartNextTurn(BlackJack::Player* player, SleepyDiscord::DiscordClient& client, SleepyDiscord::Snowflake<SleepyDiscord::Channel> channelId);
	bool CheckIfPlayerBusts(BlackJack::Player* player, SleepyDiscord::DiscordClient& client, SleepyDiscord::Snowflake<SleepyDiscord::Channel> channelId);
	void StartGame(BlackJack::Game* game, SleepyDiscord::DiscordClient& client, SleepyDiscord::Snowflake<SleepyDiscord::Channel> channelId);
	void Continue(BlackJack::Game* game, BlackJack::Player* nPlayer, SleepyDiscord::DiscordClient& client, SleepyDiscord::Snowflake<SleepyDiscord::Channel> channelId);

	void QueuePlayerToRemove(std::string channelId, BlackJack::Player player);
	void RemovePlayersFromGame(BlackJack::Game* game, std::string channelId, SleepyDiscord::DiscordClient& client);
	std::map<std::string, std::vector<BlackJack::Player>> playersToRemove;

private:
	std::map<std::string, BlackJack::Game> vGames;

};


