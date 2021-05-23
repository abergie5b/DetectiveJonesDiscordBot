#ifndef __HELPERS_H_
#define __HELPERS_H_

#include <string>
#include <string.h>
#include <vector>
#include <queue>
#include <sstream>

std::string formatMultiLineChannelText(const std::string& msg, char delim = ',');
std::string formatChannelText(const std::string& s);
std::vector<std::string> split(const std::string& s, char delim);
std::queue<std::string> split_params(const std::string& source);
std::string unravel(std::queue<std::string> queue);

#endif
