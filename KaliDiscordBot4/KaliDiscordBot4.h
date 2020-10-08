#include "sleepy_discord/sleepy_discord.h"
#include "sleepy_discord/websocketpp_websocket.h"
//
#include <iostream>
#include <random>
#include <curl/curl.h>
#include <string.h>

#include "helpers.h"

class KaliDiscordBot : public SleepyDiscord::DiscordClient
{
public:
    void onMessage(SleepyDiscord::Message message);
	KaliDiscordBot(std::string token, const char numThreads = SleepyDiscord::DEFAULT_THREADS);
	rapidjson::Document getTriviaServerResponse(std::string js);
	rapidjson::Document getTriviaQuestion(std::string category);
	rapidjson::Document getTriviaCategories(std::string source);
	rapidjson::Document getTriviaChoices();
	rapidjson::Document setTriviaDifficulty(std::string difficulty);
	rapidjson::Document getTriviaGiveUpResponse();
	rapidjson::Document getCurrentTriviaQuestion();
	rapidjson::Document getTriviaAllScores();
	rapidjson::Document getTriviaResponseJS(SleepyDiscord::Response _msg);
	rapidjson::Document updateTriviaSource(std::string source);
	rapidjson::Document getTriviaSources();
	rapidjson::Document getTriviaScores(std::string username = "");
	rapidjson::Document sendTriviaAnswer(std::string username, std::string answer);
	rapidjson::Document getChatBotMessage(std::string words);

};
