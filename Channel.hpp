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
	Channel(string name, string manager) : name(name), manager(manager) {}
	Channel& operator=(const Channel& c)
	{
		name = c.name;
		manager = c.manager;
		topic = c.topic;
		member = c.member;
		return *this;
	}
	~Channel() {}

	void sendMsg(map<int, Client> &client, int i, string msg, int self=1)
	{
		for (set<int>::iterator itr = member.begin(); itr != member.end(); ++itr)
			if (self || i != *itr)
				client[*itr].sendMsg(msg);
	}
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