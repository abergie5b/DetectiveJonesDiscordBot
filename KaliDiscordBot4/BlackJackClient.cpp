#include "BlackJackClient.h"

const std::string BlackJackClient::GAME_START_STR = "*** LET THE GAMES BEGIN FOLKS ***\n*** ALL PLAYERS ARE ANTIED UP ***\n*** GOTTA HOT HAND? ***";
const std::string BlackJackClient::GAME_DOES_NOT_EXIST_STR = "A BlackJack game does not exist in this channel\nType \"$blackjack\" to start a game";


const std::string BlackJackClient::GetHandStr(BlackJack::Player& player)
{
	std::stringstream ss;
	std::vector<BlackJack::Card> cards = player.GetHand().GetCards();
	ss << "`" << player.GetName() << "`: ";
	for (int x = 0; x < cards.size(); x++)
	{
		if (x == 1)
		{
			ss << "||";
		}
		ss << BlackJack::Cards.at(cards[x].GetName()) << " of " << BlackJack::Suites.at(cards[x].GetSuite());
		if (x == 1)
		{
			ss << "||";
		}
		if (x < cards.size() - 1)
		{
			ss << " and ";
		}
	}
	ss << std::endl;
	return ss.str();
}

const std::string BlackJackClient::GetCardStr(BlackJack::Player player, BlackJack::Card card)
{
	std::stringstream ss;
	ss << "`" << player.GetName() << "`: " << BlackJack::Cards.at(card.GetName()) << " of " << BlackJack::Suites.at(card.GetSuite());
	return ss.str();
}

const std::string BlackJackClient::GetLeaveGameStr(BlackJack::Player& player)
{
	std::stringstream ss;
	ss << player.GetName() << " has left the game!";
	return ss.str();
}

const std::string BlackJackClient::GetPlayerBustsStr(BlackJack::Player player)
{
	std::stringstream ss;
	ss << player.GetName() << " busts!!!";
	return ss.str();
}

const std::string BlackJackClient::GetDealerBustsStr()
{
	std::stringstream ss;
	ss << "Dealer busts!!!";
	return ss.str();
}

const std::string BlackJackClient::GetPlayerPassStr(BlackJack::Player player)
{
	std::stringstream ss;
	ss << player.GetName() << " stays";
	return ss.str();
}


const std::string BlackJackClient::GetPlayerStartTurnStr(BlackJack::Player player)
{
	std::stringstream ss;
	ss << "`" << player.GetName() << "`: hit ($h) or stay ($s)?";
	return ss.str();
}

const std::string BlackJackClient::GetCollectFromPlayerStr(BlackJack::Player player)
{
	std::stringstream ss;
	ss << "Dealer collects " << player.GetAnte() << " from " << player.GetName();
	return ss.str();
}

const std::string BlackJackClient::GetDistributeToPlayerStr(BlackJack::Player player)
{
	std::stringstream ss;
	ss << player.GetName() << " collects " << player.GetAnte() << " from Dealer";
	return ss.str();
}

const std::string BlackJackClient::GetRoundOverStr(BlackJack::Game* game)
{
	std::stringstream ss;
	ss << "*** ROUND OVER ***" << std::endl;
	ss << "*** PLACE YO' BETS LADIES AND GENTLEMEN ***" << std::endl;
	ss << "Minimum Ante: " << game->nMinAnte << std::endl;
	ss << "Join the game with command \"$j\"";
	return ss.str();
}

BlackJackClient::BlackJackClient()
	: vGames(std::map<std::string, BlackJack::Game>()),
	  playersToRemove(std::map<std::string, std::vector<BlackJack::Player>>())
{
}

void BlackJackClient::QueuePlayerToRemove(std::string channelId, BlackJack::Player player)
{
	if (playersToRemove.find(channelId) != playersToRemove.end())
	{
		std::vector<BlackJack::Player> players = playersToRemove.at(channelId);
		players.push_back(player);
	}
	else
	{
		playersToRemove.emplace(channelId, std::vector<BlackJack::Player> { player });
	}
}

void BlackJackClient::RemovePlayersFromGame(BlackJack::Game* game, std::string channelId, SleepyDiscord::DiscordClient& client)
{
	if (playersToRemove.find(channelId) == playersToRemove.end())
	{
		return;
	}
	else
	{
		std::vector<BlackJack::Player> players = playersToRemove.at(channelId);
		for (BlackJack::Player player : players)
		{
			game->RemovePlayer(player);
			client.sendMessage(channelId, formatMultiLineChannelText(BlackJackClient::GetLeaveGameStr(player)), SleepyDiscord::Async);
		}
		playersToRemove.erase(channelId);
	}
}

BlackJack::Player* BlackJackClient::GetNextPlayer(BlackJack::Game* game, BlackJack::Player* player)
{
	int x = 0;
	std::vector<BlackJack::Player> players = game->GetPlayers();
	for (x; x < players.size(); x++)
	{
		if (players[x] == *player)
		{
			break;
		}
	}
	if (x >= players.size()-1)
	{
		return &(game->dealer);
	}
	BlackJack::Player* nPlayer = game->GetPlayer(players[x+1].Name);
	nPlayer->HasCurrentTurn = true;
	return nPlayer;
}

void BlackJackClient::EndGame(BlackJack::Game* game, SleepyDiscord::DiscordClient& client, SleepyDiscord::Snowflake<SleepyDiscord::Channel> channelId )
{
	BlackJack::Dealer* dealer = &(game->dealer);
	for (BlackJack::Player& player : game->GetPlayers())
	{
		if (dealer->WinsAgainst(player))
		{
			client.sendMessage(channelId, formatMultiLineChannelText(BlackJackClient::GetCollectFromPlayerStr(player)), SleepyDiscord::Async);
			dealer->CollectFrom(player);
		}
		else if (dealer->LosesAgainst(player))
		{
			client.sendMessage(channelId, formatMultiLineChannelText(BlackJackClient::GetDistributeToPlayerStr(player)), SleepyDiscord::Async);
			dealer->DistributeTo(player);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	for (BlackJack::Player& player : game->GetPlayers())
	{
		player.DiscardHand();
		player.SetAnte(0);
	}
	dealer->DiscardHand();
	if (dealer->Hand.GetCards().size() <= BlackJack::DECK_SIZE / 2)
		dealer->deck.Collect();
	game->IsPlaying = false;
	this->RemovePlayersFromGame(game, channelId.string(), client);
}

void BlackJackClient::DealHands(std::string channelId)
{
	std::map<BlackJack::Player, BlackJack::Hand> hands;
	BlackJack::Game* game = this->GetGame(channelId);
	if (!game)
	{
		return;
	}

	for (auto& player : game->GetPlayers())
	{
		game->dealer.DealHit(player);
		game->dealer.DealHit(player);
	}
	game->dealer.DealHit(game->dealer);
	game->dealer.DealHit(game->dealer);
}

BlackJack::Game* BlackJackClient::GetGame(std::string channelId)
{
	BlackJack::Game* game = nullptr;
	auto search = vGames.find(channelId);
	if (search == vGames.end())
	{
		return game;
	}
	return &(search->second);
}


bool BlackJackClient::AllPlayersHaveAnted(std::string channelId)
{
	BlackJack::Game* game = this->GetGame(channelId);
	if (!game)
	{
		return false;
	}

	for (auto& player : game->GetPlayers())
	{
		if (player.GetAnte() == 0)
		{
			return false;
		}
	}
	return true;
}

std::string BlackJackClient::GetGameInfoStr(std::string channelId)
{
	std::stringstream ss;

	BlackJack::Game* game = this->GetGame(channelId);
	if (!game)
	{
		ss << "A BlackJack game does not exist in this channel" << std::endl;
		ss << "Type \"$blackjack\" to start a game";
		return ss.str();
	}

	ss << "Dealer: InspectorJones" << std::endl;
	ss << "Players: ";
	for (auto player : game->GetPlayers())
	{
		ss << player.Name << " ";
	}
	ss << std::endl;
	ss << "Minimum Ante: " << game->nMinAnte << std::endl;
	ss << "Join the game with command \"$j\"";
	return ss.str();
}

std::string BlackJackClient::CreateGame(std::string channelId, BlackJack::Player player)
{
	if (vGames.find(channelId) == vGames.end())
	{
		player.SetAnte(0); // HACKS
		BlackJack::Game game = BlackJack::Game(player);
		game.SetMinAnte(100);
		vGames.emplace(channelId, game);
		std::stringstream ss;
		ss << "*** PLACE YO' BETS LADIES AND GENTLEMAN ***" << std::endl;
		ss << "BlackJack game created by: " << player.GetName() << std::endl;
		ss << "Join the game with command \"$j\"" << std::endl;
		ss << "Dealer: InspectorJones" << std::endl;
		ss << "Minimum Ante: " << game.nMinAnte;
		return ss.str();
	}
	return "A BlackJack game already exists in this channel";
}

