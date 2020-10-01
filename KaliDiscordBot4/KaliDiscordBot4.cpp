#include <random>
#include "KaliDiscordBot4.h"

std::queue<std::string> split(const std::string& source) {
	std::stringstream ss(source);
	std::string item;
	std::queue<std::string> target;
	while (std::getline(ss, item, ' '))
		if (!item.empty())
			target.push(item);
	return target;
}


class KaliDiscordBot;

namespace Command {
	using Verb = std::function<
		void(
			KaliDiscordBot&,
			SleepyDiscord::Message&,
			std::queue<std::string>&
		)
	>;
	struct Command {
		std::string name;
		std::vector<std::string> params;
		Verb verb;
	};
	using MappedCommands = std::unordered_map<std::string, Command>;
	using MappedCommand = MappedCommands::value_type;
	static MappedCommands all;
	static void addCommand(Command command) {
		all.emplace(command.name, command);
	}
}


class KaliDiscordBot : public SleepyDiscord::DiscordClient {
public:
    using SleepyDiscord::DiscordClient::DiscordClient;
    void onMessage(SleepyDiscord::Message message) {
		if (message.isMentioned(getID()))
		{
			std::queue<std::string> parameters = split(message.content);
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
				return;

			//get command
			Command::MappedCommands::iterator foundCommand =
				Command::all.find(parameters.front());
			if (foundCommand == Command::all.end()) {
				sendMessage(message.channelID, "Error: Command not found", SleepyDiscord::Async);
				return;
			}
			parameters.pop();
			if (
				parameters.size() <
				foundCommand->second.params.size()
				) {
				sendMessage(message.channelID, "Error: Too few parameters", SleepyDiscord::Async);
				return;
			}

			//call command
			foundCommand->second.verb(*this, message, parameters);
		}
    }
};

void helloCommand(KaliDiscordBot& client, SleepyDiscord::Message& message, std::vector<std::string> greetings)
{
	std::string greeting = greetings[rand() % greetings.size()];
	std::string output = greeting + " " + message.author.username;
	client.sendMessage(message.channelID, output, SleepyDiscord::Async);
}


int main()
{
	Command::addCommand({
		"help", {}, [](
			KaliDiscordBot& client,
			SleepyDiscord::Message& message,
			std::queue<std::string>&
		) {
			constexpr char start[] = "Here's a list of all commands:```\n";
			constexpr char theEnd[] = "```";
			//estimate length
			std::size_t length = strlen(start) + strlen(theEnd);
			for (Command::MappedCommand& command : Command::all) {
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
			for (Command::MappedCommand& command : Command::all) {
				output += command.first;
				output += ' ';
				for (std::string& parmaName : command.second.params) {
					output += '<';
					output += parmaName;
					output += "> ";
				}
				output += '\n';
			}
			output += theEnd;
			client.sendMessage(message.channelID, output, SleepyDiscord::Async);
		}
	});

	std::vector<std::string> greetings = {
		"hi",
		"hello",
		"hey",
		"sup",
		"yo"
	};

	for (std::string greeting : greetings)
	{
		Command::addCommand({
			greeting, {}, [&, greetings](
				KaliDiscordBot& client,
				SleepyDiscord::Message& message,
				std::queue<std::string>&
			) {
				helloCommand(client, message, greetings);
			}
		});
	}

    KaliDiscordBot bot("NzYxMzQ2NTQwOTYyODQwNjI2.X3ZROA.khKOWw8X5MqT8lHt3kqBayiMhVw", 2);
	bot.run();

}
