#ifndef CHANNEL_HPP
# define CHANNEL_HPP

class Channel
{
public:
	string name;
	string manager;
	string topic;
	set<string> member;
};

#endif