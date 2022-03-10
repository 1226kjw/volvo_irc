#include "Client.hpp"

Client::Client() : _is_authenticated(false), _is_registered(false), _nickname(""), _username(""), _msg(""), _idx(-1), _fd(-1) {}

Client::Client(int idx, int fd) : _is_authenticated(false), _is_registered(false), _nickname(""), _username(""), _msg(""), _idx(idx), _fd(fd) {}

Client::Client(const Client& a)
{
	*this = a;

}

Client& Client::operator=(const Client& a)
{
	_is_authenticated = a._is_authenticated;
	_is_registered = a._is_registered;
	_nickname = a._nickname;
	_username = a._username;
	_msg = a._msg;
	_idx = a._idx;
	_fd = a._fd;
	return *this;
}

void Client::feed(char *buf)
{
	_msg += buf;
}

int Client::authenticate(string passwd, vector<string> arg)
{
	if (arg.size() == 1 && passwd == arg[0])
	{
		sendMsg("authenticated\n");
		_is_authenticated = true;
        return VALID;
    }
    else
		return WRONG_PW;
}

int Client::nick(map<string, int> &client_map, vector<string> args)
{
	if (args.size() != 1)
    	return NEEDMOREPARAMS;
    string arg = args[0];
	if (client_map.find(arg) != client_map.end())
		return NICKNAMEINUSE;
	// if (isin())
	_nickname = arg;
	client_map[arg] = _idx;
	if (_is_authenticated && _nickname != "" && _username != "")
	{
		_is_registered = true;
		sendMsg("registered\n");
	}
	return VALID;
}

int Client::user(vector<string> args)
{
	if (args.size() != 1)
		return NEEDMOREPARAMS;
	_username = args[0];
	if (_is_authenticated && _nickname != "" && _username != "")
	{
		_is_registered = true;
		sendMsg("registered\n");
	}
	return VALID;
}

void Client::sendMsg(string message, int flag=0)
{
	send(_fd, message.c_str(), message.size(), flag);
}

string Client::prefix(void)
{
	return string(":") + _username + ' ';
}

bool Client::is_registered(void)
{
	return _is_registered;
}

bool Client::is_authenticated(void)
{
	return _is_authenticated;
}

string Client::message(void)
{
	return _msg;
}

void Client::message(string s)
{
	_msg = s;
}

set<string>& Client::joined_channel(void)
{
	return _joined_channel;
}

string Client::nickname(void)
{
	return _nickname;
}

string Client::username(void)
{
	return _username;
}

int  Client::idx(void)
{
	return _idx;
}

int  Client::fd(void)
{
	return _fd;
}