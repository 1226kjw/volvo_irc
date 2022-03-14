#include "Channel.hpp"

using std::string;
using std::set;
using std::map;

Channel::Channel() {}

Channel::Channel(const Channel& c)
{
	*this = c;
}

Channel::Channel(string name) : _name(name) {}

Channel& Channel::operator=(const Channel& c)
{
	_name = c._name;
	_topic = c._topic;
	_member = c._member;
	return *this;
}

Channel::~Channel() {}

bool Channel::isin(int i)
{
	return _member.find(i) != _member.end();
}

void Channel::sendMsg(map<int, Client> &client, int i, string msg, int self)
{
	for (set<int>::iterator itr = _member.begin(); itr != _member.end(); ++itr)
		if (self || i != *itr)
			client[*itr].sendMsg(msg);
}

void Channel::join(Client &c)
{
	_member.insert(c.idx());
	c.joined_channel().insert(_name);
}

void Channel::out(Client& c, map<string, Channel>& channel)
{
	if (!isin(c.idx()))
		return ;
	_member.erase(c.idx());
	c.joined_channel().erase(_name);
	if (_member.size() == 0)
		channel.erase(_name);
}

set<int>& Channel::member(void)
{
	return _member;
}
