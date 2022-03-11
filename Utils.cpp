#include "Utils.hpp"

std::vector<std::string> split(std::string str, char d)
{
	std::vector<std::string> ret;
	std::string istr;
	for (std::string::iterator c = str.begin(); c != str.end(); ++c)
	{
		if (istr == "" && *c == ':')
		{
			ret.push_back(str.substr(c - str.begin(), std::string::npos));
			break ;
		}
		if (*c != d)
			istr.push_back(*c);
		else if (istr != "")
		{
			ret.push_back(istr);
			istr = "";
		}
	}
	if (istr != "")
		ret.push_back(istr);
	return ret;
}

bool isin(char c, std::string pool)
{
	return (pool.find(c) != std::string::npos);
}

bool nickcheck(std::string str)
{
	char i = str[0];
	if (((!std::isalpha(i) && !isin(i, ";[]\\`_^{|}")) || str.size() > 8))
		return false;
	for (std::string::iterator itr = ++str.begin(); itr != str.end(); ++itr)
		if (!std::isalnum(*itr) && !isin(*itr, ";[]\\`_^{|}-"))
			return false;
	return true;
}