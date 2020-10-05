#include "KaliDiscordBot4.h"

std::string formatMultiLineChannelText(const std::string& msg, char delim)
{
	constexpr char start[] = "```\n";
	constexpr char theEnd[] = "```";
	//estimate length
	std::size_t length = strlen(start) + strlen(theEnd);
	for (std::string s : split(msg, delim))
	{
		length += s.size();
		length += 2; // ' ' and '\n'
	}
	
	std::string output;
	output.reserve(length);
	output += start;
	for (std::string s : split(msg, delim))
	{
		output += s;
		output += ' ';
		output += '\n';
	}
	output += theEnd;
	return output;
}

std::string formatChannelText(const std::string& s)
{
	constexpr char start[] = "```\n";
	constexpr char theEnd[] = "```";
	//estimate length
	std::size_t length = strlen(start) + strlen(theEnd);
	length += s.size();
	length += 2; // ' ' and '\n'
	
	std::string output;
	output.reserve(length);
	output += start;
	output += s;
	output += ' ';
	output += '\n';
	output += theEnd;
	return output;
}

std::vector<std::string> split(const std::string& s, char delim)
{
	std::stringstream ss(s);
	std::string item;
	std::vector<std::string> elems;
	while (std::getline(ss, item, delim)) {
		elems.push_back(std::move(item));
	}
	return elems;
}

std::queue<std::string> split_params(const std::string& source) {
	std::stringstream ss(source);
	std::string item;
	std::queue<std::string> target;
	while (std::getline(ss, item, ' '))
		if (!item.empty())
			target.push(item);
	return target;
}

std::string unravel(std::queue<std::string> queue)
{
	std::string target = "";
	while (!queue.empty())
	{
		target += " " + queue.front();
		queue.pop();
	}
	return target;
}

namespace KaliCommand {
	using Verb = std::function<
		void(
			KaliDiscordBot&,
			SleepyDiscord::Message&,
			std::queue<std::string>&
		)
	>;
	struct KaliCommand {
		std::string name;
		std::string desc;
		std::vector<std::string> params;
		Verb verb;
	};
	using MappedCommands = std::map<std::string, KaliCommand>;
	using MappedCommand = MappedCommands::value_type;
	static MappedCommands all;
	static void addCommand(KaliCommand command) {
		all.emplace(command.name, command);
	}
}

KaliDiscordBot::KaliDiscordBot(std::string token, const char numThreads)
	: SleepyDiscord::DiscordClient(token, numThreads)
{
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

	if (message.startsWith("!")) // trivia
	{
		std::queue<std::string> parameters = split_params(message.content);
		KaliCommand::MappedCommands::iterator foundCommand = KaliCommand::all.find(parameters.front());
		if (foundCommand != KaliCommand::all.end())
			foundCommand->second.verb(*this, message, parameters);
		return;
	}

	if (message.isMentioned(getID())) // chat & trivia
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

			SleepyDiscord::Response response = getChatBotMessage(unravel(parameters));
			rapidjson::Document doc;
			const char* text = response.text.c_str();
			doc.Parse(text);
			rapidjson::Value& msg = doc["response"];
			sendMessage(message.channelID, msg.GetString(), SleepyDiscord::Async);
			return;
		}

		parameters.pop();
		if (
			parameters.size() <
			foundCommand->second.params.size()
			) {
			sendMessage(message.channelID, "What?", SleepyDiscord::Async);
			return;
		}

		// call command
		foundCommand->second.verb(*this, message, parameters);
	}
}

SleepyDiscord::Response KaliDiscordBot::getTriviaServerResponse(std::string js)
{
	SleepyDiscord::Session session;
	session.setUrl("http://127.0.0.1:5000/trivia");
	std::vector<SleepyDiscord::HeaderPair> header = {};
	header.push_back({ "Content-Type"  , "application/json" });
	session.setBody(&js);
	header.push_back({ "Content-Length", std::to_string(js.length()) });
	session.setHeader(header);
	SleepyDiscord::Response response = session.request(SleepyDiscord::Post);
	return response;
}

SleepyDiscord::Response KaliDiscordBot::getTriviaQuestion(std::string category)
{
	category = "\"" + category + "\"";
	std::string js = SleepyDiscord::json::createJSON({
		{"category", category}
	});
	return getTriviaServerResponse(js);
}

SleepyDiscord::Response KaliDiscordBot::getTriviaCategories(std::string source)
{
	std::string js = SleepyDiscord::json::createJSON({
		{"categories", "\"" + source + "\""}
	});
	return getTriviaServerResponse(js);
}

SleepyDiscord::Response KaliDiscordBot::getTriviaChoices()
{
	std::string js = SleepyDiscord::json::createJSON({
		{"choices", "\"True\""}
	});
	return getTriviaServerResponse(js);
}

SleepyDiscord::Response KaliDiscordBot::setTriviaDifficulty(std::string difficulty)
{
	std::string js = SleepyDiscord::json::createJSON({
		{"difficulty", "\"" + difficulty + "\""}
	});
	return getTriviaServerResponse(js);
}

SleepyDiscord::Response KaliDiscordBot::getTriviaGiveUpResponse()
{
	return getTriviaServerResponse(
		SleepyDiscord::json::createJSON({
			{"giveup", "\"True\""}
		})
	);
}

SleepyDiscord::Response KaliDiscordBot::getCurrentTriviaQuestion()
{
	std::string js = SleepyDiscord::json::createJSON({
		{"question", "\"True\""}
	});
	return getTriviaServerResponse(js);
}

rapidjson::Value& KaliDiscordBot::getTriviaResponseJS(SleepyDiscord::Response _msg, const char* field)
{
	rapidjson::Document doc;
	const char* text = _msg.text.c_str();
	doc.Parse(text);
	return doc[field];
}

SleepyDiscord::Response KaliDiscordBot::updateTriviaSource(std::string source)
{
	std::string js = SleepyDiscord::json::createJSON({
		{"source", "\"" + source + "\""}
	});
	return getTriviaServerResponse(js);
}

SleepyDiscord::Response KaliDiscordBot::getTriviaSources()
{
	std::string js = SleepyDiscord::json::createJSON({
		{"sources", "\"True\""}
	});
	return getTriviaServerResponse(js);
}

SleepyDiscord::Response KaliDiscordBot::getTriviaScores(std::string username)
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

SleepyDiscord::Response KaliDiscordBot::sendTriviaAnswer(std::string username, std::string answer)
{
	username = "\"" + username + "\"";
	answer = "\"" + answer + "\"";
	std::string js = SleepyDiscord::json::createJSON({
		{"username", username},
		{"answer", answer}
	});
	return getTriviaServerResponse(js);
}

SleepyDiscord::Response KaliDiscordBot::getChatBotMessage(std::string words)
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
	return response;
}


void addCommands()
{
	KaliCommand::addCommand({
		"help", "displays this message", {}, [](
			KaliDiscordBot& client,
			SleepyDiscord::Message& message,
			std::queue<std::string>&
		) {
			constexpr char start[] = "Here's a list of all commands (trivia commands start with '!'):```\n";
			constexpr char theEnd[] = "```";
			//estimate length
			std::size_t length = strlen(start) + strlen(theEnd);
			for (KaliCommand::MappedCommand& command : KaliCommand::all) {
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
				SleepyDiscord::Response response = client.getTriviaQuestion(command);
				//rapidjson::Value& msg = client.getTriviaResponseJS(response, "response");

				rapidjson::Document doc;
				const char* text = response.text.c_str();
				doc.Parse(text);
				rapidjson::Value& msg = doc["response"];

				client.sendMessage(message.channelID, formatChannelText(msg.GetString()), SleepyDiscord::Async);
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
				SleepyDiscord::Response response = client.getCurrentTriviaQuestion();
				//rapidjson::Value& msg = client.getTriviaResponseJS(response, "response");
				rapidjson::Document doc;
				const char* text = response.text.c_str();
				doc.Parse(text);
				rapidjson::Value& msg = doc["response"];

				client.sendMessage(message.channelID, formatChannelText(msg.GetString()), SleepyDiscord::Async);
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
				SleepyDiscord::Response response = client.updateTriviaSource(source);
				//rapidjson::Value& msg = client.getTriviaResponseJS(response, "response");

				rapidjson::Document doc;
				const char* text = response.text.c_str();
				doc.Parse(text);
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
				SleepyDiscord::Response response = client.getTriviaGiveUpResponse();
				//rapidjson::Value& msg = client.getTriviaResponseJS(response, "response");
				rapidjson::Document doc;
				const char* text = response.text.c_str();
				doc.Parse(text);
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
				SleepyDiscord::Response response = client.sendTriviaAnswer(message.author.username, answer);
				//rapidjson::Value& msg = client.getTriviaResponseJS(response, "response");
				rapidjson::Document doc;
				const char* text = response.text.c_str();
				doc.Parse(text);
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
				SleepyDiscord::Response response = client.getTriviaChoices();
				//rapidjson::Value& msg = client.getTriviaResponseJS(response, "response");
				rapidjson::Document doc;
				const char* text = response.text.c_str();
				doc.Parse(text);
				rapidjson::Value& msg = doc["response"];
				client.sendMessage(message.channelID, formatMultiLineChannelText(msg.GetString(), ';'), SleepyDiscord::Async);
			}
		});
	}

	for (std::string s: {"!s", "!scores"})
	{
		KaliCommand::addCommand({
			s, "displays trivia high scores", {}, [](
				KaliDiscordBot& client,
				SleepyDiscord::Message& message,
				std::queue<std::string>& params
			) {
				std::string answer = unravel(params);
				SleepyDiscord::Response response = client.getTriviaScores(message.author.username);
				//rapidjson::Value& msg = client.getTriviaResponseJS(response, "response");
				rapidjson::Document doc;
				const char* text = response.text.c_str();
				doc.Parse(text);
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
				SleepyDiscord::Response response = client.getTriviaSources();
				//rapidjson::Value& msg = client.getTriviaResponseJS(response, "response");
				rapidjson::Document doc;
				const char* text = response.text.c_str();
				doc.Parse(text);
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
				SleepyDiscord::Response response = client.setTriviaDifficulty(difficulty);
				//rapidjson::Value& msg = client.getTriviaResponseJS(response, "response");
				rapidjson::Document doc;
				const char* text = response.text.c_str();
				doc.Parse(text);
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
			SleepyDiscord::Response response = client.getTriviaCategories(source);
			//rapidjson::Value& msg = client.getTriviaResponseJS(response, "response");
			rapidjson::Document doc;
			const char* text = response.text.c_str();
			doc.Parse(text);
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

