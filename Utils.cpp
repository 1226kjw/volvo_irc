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
	if (((!std::isalpha(i) && !isin(i, ";[]\\`_^{|}")) || str.size() > 9))
		return false;
	for (std::string::iterator itr = ++str.begin(); itr != str.end(); ++itr)
		if (!std::isalnum(*itr) && !isin(*itr, ";[]\\`_^{|}-"))
			return false;
	return true;
}

int irc_atoi(std::string str)
{
	long long int ret = 0;

	for (int i = 0; str[i] && ret >= 0; ++i)
		if (std::isdigit(str[i]))
			ret = ret * 10 + str[i] - '0';
		else
			throw std::invalid_argument("");

	if ((ret < 0) || (ret > 2147483647))
		throw std::out_of_range("");
	return static_cast<int>(ret);
}
