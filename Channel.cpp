#include "Channel.hpp"

Channel::Channel() {}

Channel::Channel(const Channel& c)
{
	*this = c;
}

Channel::Channel(string name, string manager) : _name(name), _manager(manager) {}

Channel& Channel::operator=(const Channel& c)
{
	_name = c._name;
	_manager = c._manager;
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

void Channel::out(Client& c)
{
	if (!isin(c.idx()))
		return ;
	_member.erase(c.idx());
	c.joined_channel().erase(_name);
}

string Channel::manager()
{
	return _manager;
}