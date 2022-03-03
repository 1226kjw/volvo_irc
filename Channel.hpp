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

	Channel() {}
	Channel(const Channel& c)
	{
		*this = c;
	}
	Channel(string name) : name(name) {}
	Channel& operator=(const Channel& c)
	{
		name = c.name;
		manager = c.manager;
		topic = c.topic;
		member = c.member;
		return *this;
	}
	~Channel() {}

	void join(Client &c)
	{
		member.insert(c.idx);
		c.joined_channel.insert(name);
	}
	void out(Client& c)
	{
		member.erase(c.idx);
		c.joined_channel.erase(name);
	}
};

#endif