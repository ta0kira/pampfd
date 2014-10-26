#include "whitelist.hpp"

#include <string>
#include <list>
#include <fstream>

#include "regex-check.hpp"


typedef std::list <regex_check> pattern_list;

std::string filename;
static pattern_list patterns;


bool set_whitelist(const std::string &name)
{
	filename = name;
	return reload_whitelist();
}


bool reload_whitelist()
{
	std::fstream input(filename.c_str(), std::ios::in);
	if (!input) return false;

	patterns.clear();

	char buffer[BUFFER_SIZE] = { 0x00 };
	while (input.getline(buffer, sizeof buffer))
	{
	regex_check new_pattern(false);
	if (!(new_pattern = buffer)) return false;
	patterns.push_back(new_pattern);
	}

	return true;
}


bool check_whitelisted(const std::string &address)
{
	for (pattern_list::const_iterator current = patterns.begin(), end = patterns.end();
	     current != end; ++current)
	if (*current == address.c_str()) return true;

	return false;
}
