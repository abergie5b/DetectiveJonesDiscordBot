#include "KaliDiscordBot4.h"
#include "KaliCommand.h"

#include <thread>

KaliDiscordBot::KaliDiscordBot(std::string token, const char numThreads)
	: SleepyDiscord::DiscordClient(token, numThreads),
	  bjClient(BlackJackClient()),
	  bjUserTimeoutList(std::vector<std::string>())
{
}

std::string KaliDiscordBot::CreateBJGame(const std::string& channelId, const std::string& userId)
{
	return this->bjClient.CreateGame(channelId, BlackJack::Player(userId));
}

std::string KaliDiscordBot::JoinBJGame(const std::string& channelId, const std::string& userId)
{
	BlackJack::Game* game = this->bjClient.GetGame(channelId);
	if (game)
	{
		std::stringstream ss;
		std::vector<BlackJack::Player> players = game->GetPlayers();
		for (BlackJack::Player player : players)
		{
			if (player.GetName() == userId)
			{
				ss << userId << " is already playing in this channel";
				return ss.str();
			}
		}
		BlackJack::Player player = BlackJack::Player(userId);
		game->AddPlayer(player);
		ss << "Added " << userId << " to the game";
		return ss.str();
	}
	return BlackJackClient::GAME_DOES_NOT_EXIST_STR;
}

void KaliDiscordBot::bjRemoveUsersForInactivity()
{
	if (bjUserTimeoutList.empty())
	{
		return;
	}

	std::map<std::string, BlackJack::Game> games = this->bjClient.GetGames();
	auto it = games.begin();
	while (it != games.end())
	{
		std::vector<BlackJack::Player> players = it->second.GetPlayers();
		std::vector<std::string> userIds;
		for (BlackJack::Player player : players)
		{
			for (std::string userId : bjUserTimeoutList)
			{
				if (std::find(userIds.begin(), userIds.end(), player.Name) != userIds.end())
				{
					bjClient.QueuePlayerToRemove(it->first, player);
					bjClient.RemovePlayersFromGame(&(it->second), it->first, *this);
				}
			}
		}
		it++;
	}
}

std::string KaliDiscordBot::MakeAnte(BlackJack::Game* game, const std::string& userId, const uint32_t ante)
{
	std::stringstream ss;

	if (ante < game->nMinAnte)
	{
		ss << userId << " your ante (" << ante << ") is less than the minimum (" << game->nMinAnte << ")";
		return ss.str();
	}

	for (auto& player : game->GetPlayers())
	{
		if (player.GetName() == userId)
		{
			player.SetAnte(ante);
			ss << userId << " throws down " << ante;
			return ss.str();
		}
	}
	ss << "Type \"$j\" to join the game before placing bets";
	return ss.str();
}

std::string KaliDiscordBot::LeaveBJGame(const std::string& channelId, const std::string& userId)
{

	return "";
}

std::string KaliDiscordBot::TakeBJHit(const std::string& channelId, const std::string& userId)
{

	return "";
}

std::string KaliDiscordBot::StayBJ(const std::string& channelId, const std::string& userId)
{

	return "";
}

std::string KaliDiscordBot::GetBJCurrentGameInfo(const std::string& channelId)
{

	return "";
}

void KaliDiscordBot::onMessage(SleepyDiscord::Message message) {

	SleepyDiscord::Channel channel = getChannel(message.channelID);
	printf("%s:%s\n", message.author.ID.string().c_str(), message.author.username.c_str());

	if (message.startsWith("fok")) // reaction
	{
		char* curlStr = curl_easy_escape(nullptr, u8"🧐", 0);
		addReaction(message.channelID, message.ID, curlStr, SleepyDiscord::Async);
		curl_free(curlStr);
	}

	if (message.content.find("diabs") != std::string::npos || message.content.find("diablo") != std::string::npos)
	{
		char* curlStr = curl_easy_escape(nullptr, u8"🕵️‍♂️", 0);
		addReaction(message.channelID, message.ID, curlStr, SleepyDiscord::Async);
		curl_free(curlStr);
	}

	if (message.startsWith("!") || message.startsWith("$")) // trivia & blackjack commands
	{
		std::queue<std::string> parameters = split_params(message.content);
		KaliCommand::MappedCommands::iterator foundCommand = KaliCommand::all.find(parameters.front());
		if (foundCommand != KaliCommand::all.end())
			foundCommand->second.verb(*this, message, parameters);
		return;
	}

	if (message.isMentioned(getID())) // chat
	{
		std::queue<std::string> parameters = split_params(message.content);
		const std::string mention = "<@" + getID().string() + ">";
		const std::string mentionNick = "<@!" + getID().string() + ">";

		if (
			//only allow if has more then 1 parameter 
			parameters.size() <= 1 &&
			//only allow if starts with a mention
			(parameters.front() != mention || parameters.front() != mentionNick)
			)
			return;

		//remove the parameters as we go
		parameters.pop();
		if (parameters.empty())
		{
			return;
		}

		//get command
		KaliCommand::MappedCommands::iterator foundCommand = KaliCommand::all.find(parameters.front());
		if (foundCommand == KaliCommand::all.end()) {

			rapidjson::Document doc = getChatBotMessage(unravel(parameters));
			rapidjson::Value& msg = doc["response"];
			sendMessage(message.channelID, msg.GetString(), SleepyDiscord::Async);
			return;
		}

		parameters.pop();
		if (parameters.size() < foundCommand->second.params.size())
		{
			sendMessage(message.channelID, "What?", SleepyDiscord::Async);
			return;
		}

		// call command
		foundCommand->second.verb(*this, message, parameters);
	}
}

rapidjson::Document KaliDiscordBot::getTriviaServerResponse(std::string js)
{
	SleepyDiscord::Session session;
	session.setUrl("http://127.0.0.1:5000/trivia");
	std::vector<SleepyDiscord::HeaderPair> header = {};
	header.push_back({ "Content-Type"  , "application/json" });
	session.setBody(&js);
	header.push_back({ "Content-Length", std::to_string(js.length()) });
	session.setHeader(header);
	SleepyDiscord::Response response = session.request(SleepyDiscord::Post);
	if (response.statusCode == 0) // curl -> url not found
		response.text = "{\"response\": \"Trivia server is not running\"}";
	return getTriviaResponseJS(response);
}

rapidjson::Document KaliDiscordBot::getTriviaQuestion(std::string category)
{
	category = "\"" + category + "\"";
	std::string js = SleepyDiscord::json::createJSON({
		{"category", category}
	});
	return getTriviaServerResponse(js);
}

rapidjson::Document KaliDiscordBot::getTriviaCategories(std::string source)
{
	std::string js = SleepyDiscord::json::createJSON({
		{"categories", "\"" + source + "\""}
	});
	return getTriviaServerResponse(js);
}

rapidjson::Document KaliDiscordBot::getTriviaAllScores()
{
	std::string js = SleepyDiscord::json::createJSON({
		{"scores", "\"True\""}
	});
	return getTriviaServerResponse(js);
}

rapidjson::Document KaliDiscordBot::getTriviaChoices()
{
	std::string js = SleepyDiscord::json::createJSON({
		{"choices", "\"True\""}
	});
	return getTriviaServerResponse(js);
}

rapidjson::Document KaliDiscordBot::setTriviaDifficulty(std::string difficulty)
{
	std::string js = SleepyDiscord::json::createJSON({
		{"difficulty", "\"" + difficulty + "\""}
	});
	return getTriviaServerResponse(js);
}

rapidjson::Document KaliDiscordBot::getTriviaGiveUpResponse()
{
	return getTriviaServerResponse(
		SleepyDiscord::json::createJSON({
			{"giveup", "\"True\""}
		})
	);
}

rapidjson::Document KaliDiscordBot::getCurrentTriviaQuestion()
{
	std::string js = SleepyDiscord::json::createJSON({
		{"question", "\"True\""}
	});
	return getTriviaServerResponse(js);
}

rapidjson::Document KaliDiscordBot::getTriviaResponseJS(SleepyDiscord::Response _msg)
{
	rapidjson::Document doc;
	const char* text = _msg.text.c_str();
	doc.Parse(text);
	return doc;
}

rapidjson::Document KaliDiscordBot::updateTriviaSource(std::string source)
{
	std::string js = SleepyDiscord::json::createJSON({
		{"source", "\"" + source + "\""}
	});
	return getTriviaServerResponse(js);
}

rapidjson::Document KaliDiscordBot::getTriviaSources()
{
	std::string js = SleepyDiscord::json::createJSON({
		{"sources", "\"True\""}
	});
	return getTriviaServerResponse(js);
}

rapidjson::Document KaliDiscordBot::getTriviaScores(std::string username)
{
	std::string js = "";
	if (username != "")
	{
		username = "\"" + username + "\"";
		js = SleepyDiscord::json::createJSON({
			{"scores", username},
			{"username", username}
		});
	}
	else
	{
		username = "\"" + username + "\"";
		js = SleepyDiscord::json::createJSON({
			{"scores", "\"True\""}
		});
	}
	return getTriviaServerResponse(js);
}

rapidjson::Document KaliDiscordBot::sendTriviaAnswer(std::string username, std::string answer)
{
	username = "\"" + username + "\"";
	answer = "\"" + answer + "\"";
	std::string js = SleepyDiscord::json::createJSON({
		{"username", username},
		{"answer", answer}
	});
	return getTriviaServerResponse(js);
}

rapidjson::Document KaliDiscordBot::getChatBotMessage(std::string words)
{ 
	SleepyDiscord::Session session;
	session.setUrl("http://127.0.0.1:5000/chat");
	std::vector<SleepyDiscord::HeaderPair> header = {};
	header.push_back({ "Content-Type"  , "application/json" });
	words = "\"" + words + "\"";
	std::string js = SleepyDiscord::json::createJSON({
		{"words", words}
	});
	session.setBody(&js);
	header.push_back({ "Content-Length", std::to_string(js.length()) });
	session.setHeader(header);
	SleepyDiscord::Response response = session.request(SleepyDiscord::Post);
	return getTriviaResponseJS(response);
}


void addCommands()
{
	KaliCommand::addCommand({
		"help", "displays this message", {}, [](
			KaliDiscordBot& client,
			SleepyDiscord::Message& message,
			std::queue<std::string>&
		) {
			constexpr char start[] = "Here's a list of all commands (type \"$help\" for blackjack commands):```\n";
			constexpr char theEnd[] = "```";
			//estimate length
			std::size_t length = strlen(start) + strlen(theEnd);
			for (KaliCommand::MappedCommand& command : KaliCommand::all) {
				if (command.first.front() == '$')
					continue;
				length += command.first.size();
				length += 2; // ' ' and '\n'
				for (std::string& parmaName : command.second.params) {
					length += 2; // '<' and '> '
					length += parmaName.size();
				}
			}
			
			std::string output;
			output.reserve(length);
			output += start;
			for (KaliCommand::MappedCommand& command : KaliCommand::all) {
				if (command.first.front() == '$')
					continue;
				output += command.first;
				output += ' ';
				for (std::string& parmaName : command.second.params) {
					output += '<';
					output += parmaName;
					output += "> ";
				}
				output += "\n\t: " + command.second.desc;
				output += '\n';
			}
			output += theEnd;
			client.sendMessage(message.channelID, output, SleepyDiscord::Async);
		}
	});


	KaliCommand::addCommand({
		"$help", "displays this message", {}, [](
			KaliDiscordBot& client,
			SleepyDiscord::Message& message,
			std::queue<std::string>&
		) {
			constexpr char start[] = "Here's a list of blackjack commands:```\n";
			constexpr char theEnd[] = "```";
			//estimate length
			std::size_t length = strlen(start) + strlen(theEnd);
			for (KaliCommand::MappedCommand& command : KaliCommand::all) {
				if (command.first.front() != '$')
					continue;
				length += command.first.size();
				length += 2; // ' ' and '\n'
				for (std::string& parmaName : command.second.params) {
					length += 2; // '<' and '> '
					length += parmaName.size();
				}
			}
			
			std::string output;
			output.reserve(length);
			output += start;
			for (KaliCommand::MappedCommand& command : KaliCommand::all) {
				if (command.first.front() != '$')
					continue;
				output += command.first;
				output += ' ';
				for (std::string& parmaName : command.second.params) {
					output += '<';
					output += parmaName;
					output += "> ";
				}
				output += "\n\t: " + command.second.desc;
				output += '\n';
			}
			output += theEnd;
			client.sendMessage(message.channelID, output, SleepyDiscord::Async);
		}
	});

	for (std::string s : {"$b", "$blackjack"})
	{
		KaliCommand::addCommand({
			s, "starts a new blackjack game", {}, [](
				KaliDiscordBot& client,
				SleepyDiscord::Message& message,
				std::queue<std::string>& params
			) {
				std::string response = client.CreateBJGame(message.channelID.string(), message.author.username);
				client.sendMessage(message.channelID, formatMultiLineChannelText(response), SleepyDiscord::Async);
			}
		});
	}

	for (std::string s : {"$j", "$join"})
	{
		KaliCommand::addCommand({
			s, "join the current blackjack game", {}, [](
				KaliDiscordBot& client,
				SleepyDiscord::Message& message,
				std::queue<std::string>& params
			) {
				
				std::string response = client.JoinBJGame(message.channelID.string(), message.author.username);
				client.sendMessage(message.channelID, formatMultiLineChannelText(response), SleepyDiscord::Async);
			}
		});
	}

	for (std::string s : {"$a", "$ante"})
	{
		KaliCommand::addCommand({
			s, "place a bet on the table", {}, [](
				KaliDiscordBot& client,
				SleepyDiscord::Message& message,
				std::queue<std::string>& params
			) {

				BlackJack::Game* game = client.bjClient.GetGame(message.channelID.string());
				if (!game)
				{
					client.sendMessage(message.channelID, 
						formatMultiLineChannelText(BlackJackClient::GAME_DOES_NOT_EXIST_STR), 
						SleepyDiscord::Async);
					return;
				}
				if (game->IsPlaying)
				{
					return;
				}

				// pop the $ command
				params.pop();
				//
				if (params.empty())
				{
					client.sendMessage(message.channelID, 
						formatMultiLineChannelText("How much would you like to bet? ($a 100)"), 
						SleepyDiscord::Async);
					return;
				}
				const std::string command = params.front();
				params.pop();
				
				std::string response = client.MakeAnte(game, message.author.username, std::stoi(command, nullptr, 10));
				client.sendMessage(message.channelID, formatMultiLineChannelText(response), SleepyDiscord::Async);

				std::this_thread::sleep_for(std::chrono::milliseconds(500));

				if (client.bjClient.AllPlayersHaveAnted(message.channelID.string()))
				{
					client.bjClient.StartGame(game, client, message.channelID);
				}

			}
		});
	}


	for (std::string s : {"$h", "$hit"})
	{
		KaliCommand::addCommand({
			s, "take a hit from the dealer", {}, [](
				KaliDiscordBot& client,
				SleepyDiscord::Message& message,
				std::queue<std::string>& params
			) {
				
				BlackJack::Game* game = client.bjClient.GetGame(message.channelID.string());
				if (!game)
				{
					client.sendMessage(message.channelID, formatMultiLineChannelText(BlackJackClient::GAME_DOES_NOT_EXIST_STR), SleepyDiscord::Async);
					return;
				}
				
				BlackJack::Player* player = game->GetPlayer(message.author.username);
				if (!player || !player->HasCurrentTurn)
				{
					return;
				}

				game->dealer.DealHit(*player);
				client.sendMessage(message.channelID, BlackJackClient::GetHandStr(*player), SleepyDiscord::Async);
				std::this_thread::sleep_for(std::chrono::milliseconds(500));

				if (client.bjClient.CheckIfPlayerBusts(player, client, message.channelID))
				{
					BlackJack::Player* nPlayer = client.bjClient.GetNextPlayer(game, player);
					client.bjClient.Continue(game, nPlayer, client, message.channelID);
				}

			}
		});
	}

	for (std::string s : {"$s", "$stay"})
	{
		KaliCommand::addCommand({
			s, "end your turn with your current hand", {}, [](
				KaliDiscordBot& client,
				SleepyDiscord::Message& message,
				std::queue<std::string>& params
			) {
				
				BlackJack::Game* game = client.bjClient.GetGame(message.channelID.string());
				if (!game)
				{
					client.sendMessage(message.channelID, formatMultiLineChannelText(BlackJackClient::GAME_DOES_NOT_EXIST_STR), SleepyDiscord::Async);
					return;
				}
				
				BlackJack::Player* player = game->GetPlayer(message.author.username);
				if (!player || !player->HasCurrentTurn)
				{
					return;
				}

				player->HasCurrentTurn = false;
				client.sendMessage(message.channelID, formatMultiLineChannelText(BlackJackClient::GetPlayerPassStr(*player)), SleepyDiscord::Async);
				std::this_thread::sleep_for(std::chrono::milliseconds(500));

				BlackJack::Player* nPlayer = client.bjClient.GetNextPlayer(game, player);
				client.bjClient.Continue(game, nPlayer, client, message.channelID);
			}
		});
	}
	for (std::string s : {"$l", "$leave"})
	{
		KaliCommand::addCommand({
			s, "leave the current blackjack game", {}, [](
				KaliDiscordBot& client,
				SleepyDiscord::Message& message,
				std::queue<std::string>& params
			) {
				BlackJack::Game* game = client.bjClient.GetGame(message.channelID.string());
				if (!game)
				{
					client.sendMessage(message.channelID, formatMultiLineChannelText(BlackJackClient::GAME_DOES_NOT_EXIST_STR), SleepyDiscord::Async);
					return;
				}
				BlackJack::Player* player = game->GetPlayer(message.author.username);
				client.bjClient.QueuePlayerToRemove(message.channelID.string(), *player);

				std::stringstream ss;
				ss << player->GetName() << " is leaving after this round";
				client.sendMessage(message.channelID, formatMultiLineChannelText(ss.str()), SleepyDiscord::Async);
			}
		});
	}


	for (std::string s : {"!test"})
	{
		KaliCommand::addCommand({
			s, " test command", {}, [](
				KaliDiscordBot& client,
				SleepyDiscord::Message& message,
				std::queue<std::string>& params
			) {

			}
		});
	}


	for (std::string s : {"!t", "!trivia"})
	{
		KaliCommand::addCommand({
			s, "returns a new trivia question (category)", {}, [](
				KaliDiscordBot& client,
				SleepyDiscord::Message& message,
				std::queue<std::string>& params
			) {
				// pop the ! command
				params.pop();
				//
				std::string command;
				if (params.empty())
					command = "random";
				else
				{
					command = params.front();
					params.pop();
				}
				rapidjson::Document doc = client.getTriviaQuestion(command);
				rapidjson::Value& msg = doc["response"];
				client.sendMessage(message.channelID, formatMultiLineChannelText(msg.GetString(), ';'), SleepyDiscord::Async);
			}
		});
	}

	for (std::string s : {"!q", "!question"})
	{
		KaliCommand::addCommand({
			s, "displays the current trivia question", {}, [](
				KaliDiscordBot& client,
				SleepyDiscord::Message& message,
				std::queue<std::string>& params
			) {
				rapidjson::Document doc = client.getCurrentTriviaQuestion();
				rapidjson::Value& msg = doc["response"];
				client.sendMessage(message.channelID, formatMultiLineChannelText(msg.GetString(), ';'), SleepyDiscord::Async);
			}
		});
	}

	for (std::string s : {"!source"})
	{
		KaliCommand::addCommand({
			s, "updates the source of trivia questions", {}, [](
				KaliDiscordBot& client,
				SleepyDiscord::Message& message,
				std::queue<std::string>& params
			) {
				// pop the ! command
				params.pop();
				std::string source;
				if (params.empty())
					source = "current";
				else
					source = params.front();
				rapidjson::Document doc = client.updateTriviaSource(source);
				rapidjson::Value& msg = doc["response"];
				client.sendMessage(message.channelID, formatChannelText(msg.GetString()), SleepyDiscord::Async);
			}
		});
	}


	for (std::string s : {"!g", "!giveup"})
	{
		KaliCommand::addCommand({
			s, "displays the answer to the trivia question", {}, [](
				KaliDiscordBot& client,
				SleepyDiscord::Message& message,
				std::queue<std::string>& params
			) {
				rapidjson::Document doc = client.getTriviaGiveUpResponse();
				rapidjson::Value& msg = doc["response"];
				client.sendMessage(message.channelID, formatChannelText(msg.GetString()), SleepyDiscord::Async);
			}
		});
	}

	for (std::string s: {"!a", "!answer"})
	{
		KaliCommand::addCommand({
			s, "submits an answer to trivia question", {"answer"}, [](
				KaliDiscordBot& client,
				SleepyDiscord::Message& message,
				std::queue<std::string>& params
			) {
				// pop the ! command
				params.pop();
				//
				std::string answer = unravel(params);
				rapidjson::Document doc = client.sendTriviaAnswer(message.author.username, answer);
				rapidjson::Value& msg = doc["response"];
				client.sendMessage(message.channelID, formatChannelText(msg.GetString()), SleepyDiscord::Async);
			}
		});
	}

	for (std::string s: {"!c", "!choices"})
	{
		KaliCommand::addCommand({
			s, "displays trivia multiple choices", {}, [](
				KaliDiscordBot& client,
				SleepyDiscord::Message& message,
				std::queue<std::string>& params
			) {
				std::string answer = unravel(params);
				rapidjson::Document doc = client.getTriviaChoices();
				rapidjson::Value& msg = doc["response"];
				client.sendMessage(message.channelID, formatMultiLineChannelText(msg.GetString(), ';'), SleepyDiscord::Async);
			}
		});
	}

	for (std::string s: {"!s", "!score"})
	{
		KaliCommand::addCommand({
			s, "displays trivia high scores", {}, [](
				KaliDiscordBot& client,
				SleepyDiscord::Message& message,
				std::queue<std::string>& params
			) {
				std::string answer = unravel(params);
				rapidjson::Document doc = client.getTriviaScores(message.author.username);
				rapidjson::Value& msg = doc["response"];
				client.sendMessage(message.channelID, formatChannelText(msg.GetString()), SleepyDiscord::Async);
			}
		});
	}

	for (std::string s: {"!sources"})
	{
		KaliCommand::addCommand({
			s, "displays trivia sources", {}, [](
				KaliDiscordBot& client,
				SleepyDiscord::Message& message,
				std::queue<std::string>& params
			) {
				rapidjson::Document doc = client.getTriviaSources();
				rapidjson::Value& msg = doc["response"];
				client.sendMessage(message.channelID, formatChannelText(msg.GetString()), SleepyDiscord::Async);
			}
		});
	}

	for (std::string s: {"!d", "!difficulty"})
	{
		KaliCommand::addCommand({
			s, "changes the trivia difficulty (easy, medium, hard)", {"difficulty"}, [](
				KaliDiscordBot& client,
				SleepyDiscord::Message& message,
				std::queue<std::string>& params
			) {
				if (params.empty())
					return;
				std::string difficulty = params.front();
				params.pop();
				rapidjson::Document doc = client.setTriviaDifficulty(difficulty);
				rapidjson::Value& msg = doc["response"];
				client.sendMessage(message.channelID, formatChannelText(msg.GetString()), SleepyDiscord::Async);
			}
		});
	}

	KaliCommand::addCommand({
		"!categories", "displays trivia categories", {"source"}, [](
			KaliDiscordBot& client,
			SleepyDiscord::Message& message,
			std::queue<std::string>& params
		) {
			// pop the ! command
			params.pop();
			std::string source;
			if (params.empty())
				source = "current";
			else
				source = params.front();
			rapidjson::Document doc = client.getTriviaCategories(source);
			rapidjson::Value& msg = doc["response"];
			client.sendMessage(message.channelID, formatMultiLineChannelText(msg.GetString()), SleepyDiscord::Async);
		}
	});

	KaliCommand::addCommand({
		"!scores", "displays all trivia scores", {"source"}, [](
			KaliDiscordBot& client,
			SleepyDiscord::Message& message,
			std::queue<std::string>& params
		) {
			// pop the ! command
			rapidjson::Document doc = client.getTriviaAllScores();
			rapidjson::Value& msg = doc["response"];
			client.sendMessage(message.channelID, formatMultiLineChannelText(msg.GetString()), SleepyDiscord::Async);
		}
	});
}


int main()
{
    KaliDiscordBot bot("NzYxMzQ2NTQwOTYyODQwNjI2.X3ZROA.khKOWw8X5MqT8lHt3kqBayiMhVw", 2);
	addCommands();
	bot.run();
}

