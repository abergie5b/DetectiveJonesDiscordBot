#include "helpers.h"

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

