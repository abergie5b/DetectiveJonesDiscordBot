#include "sleepy_discord/sleepy_discord.h"
#include "sleepy_discord/websocketpp_websocket.h"
#include <iostream>
#include <random>
#include <curl/curl.h>
#include <string.h>

std::string formatMultiLineChannelText(const std::string& msg, char delim = ',');
std::string formatChannelText(const std::string& s);
std::vector<std::string> split(const std::string& s, char delim);
std::queue<std::string> split_params(const std::string& source);
std::string unravel(std::queue<std::string> queue);


class KaliDiscordBot : public SleepyDiscord::DiscordClient
{
public:
	int prevErrorMsg = -1;
    void onMessage(SleepyDiscord::Message message);
	KaliDiscordBot(std::string token, const char numThreads = SleepyDiscord::DEFAULT_THREADS);
	SleepyDiscord::Response getTriviaServerResponse(std::string js);
	SleepyDiscord::Response getTriviaQuestion(std::string category);
	SleepyDiscord::Response getTriviaCategories(std::string source);
	SleepyDiscord::Response getTriviaChoices();
	SleepyDiscord::Response setTriviaDifficulty(std::string difficulty);
	SleepyDiscord::Response getTriviaGiveUpResponse();
	SleepyDiscord::Response getCurrentTriviaQuestion();
	rapidjson::Value& getTriviaResponseJS(SleepyDiscord::Response _msg, const char* field = "response");
	SleepyDiscord::Response updateTriviaSource(std::string source);
	SleepyDiscord::Response getTriviaSources();
	SleepyDiscord::Response getTriviaScores(std::string username = "");
	SleepyDiscord::Response sendTriviaAnswer(std::string username, std::string answer);
	SleepyDiscord::Response getChatBotMessage(std::string words);

};
