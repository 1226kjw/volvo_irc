#include "Utils.hpp"

vector<string> split(string str, char d=' ')
{
	vector<string> ret;
	string istr;
	for (string::iterator c = str.begin(); c != str.end(); ++c)
	{
		if (istr == "" && *c == ':')
		{
			ret.push_back(str.substr(c - str.begin(), string::npos));
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

bool isin(char c, string pool)
{
	return (pool.find(c) != string::npos);
}
