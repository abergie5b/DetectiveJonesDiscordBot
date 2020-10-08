#include <string>
#include <functional>
#include <vector>
#include <map>
#include <queue>

#include "sleepy_discord/sleepy_discord.h"

class KaliDiscordBot;

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

