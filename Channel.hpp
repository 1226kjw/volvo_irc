#ifndef CHANNEL_HPP
# define CHANNEL_HPP

using std::string;

class Channel
{
public:
	string name;
	string manager;
	string topic;
	set<int> member;
};

#endif