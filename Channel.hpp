#ifndef CHANNEL_HPP
# define CHANNEL_HPP

using std::string;

class Channel
{
private:
	string _name;
	string _manager;
	string _topic;
	set<int> _member;

public:
	Channel() {}
	Channel(const Channel& c)
	{
		*this = c;
	}
	Channel(string name, string manager) : _name(name), _manager(manager) {}
	Channel& operator=(const Channel& c)
	{
		_name = c._name;
		_manager = c._manager;
		_topic = c._topic;
		_member = c._member;
		return *this;
	}
	~Channel() {}

	bool isin(int i)
	{
		return _member.find(i) != _member.end();
	}
	void sendMsg(map<int, Client> &client, int i, string msg, int self=1)
	{
		for (set<int>::iterator itr = _member.begin(); itr != _member.end(); ++itr)
			if (self || i != *itr)
				client[*itr].sendMsg(msg);
	}
	void join(Client &c)
	{
		_member.insert(c.idx());
		c.joined_channel().insert(_name);
	}
	void out(Client& c)
	{
		if (!isin(c.idx()))
			return ;
		_member.erase(c.idx());
		c.joined_channel().erase(_name);
	}
	string manager()
	{
		return _manager;
	}
};

#endif