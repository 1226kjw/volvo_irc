#include "Client.hpp"

using std::string;
using std::vector;
using std::map;
using std::set;

Client::Client() : _is_authenticated(false), _is_registered(false), _nickname(""), _username(""), _msg(""), _idx(-1), _fd(-1), _mode(0) {}

Client::Client(int idx, int fd) : _is_authenticated(false), _is_registered(false), _nickname(""), _username(""), _msg(""), _idx(idx), _fd(fd), _mode(0) {}

Client::Client(const Client& a)
{
	*this = a;
}

Client::~Client() {}


Client& Client::operator=(const Client& a)
{
	_is_authenticated = a._is_authenticated;
	_is_registered = a._is_registered;
	_nickname = a._nickname;
	_username = a._username;
	_msg = a._msg;
	_idx = a._idx;
	_fd = a._fd;
	_mode = a._mode;
	return *this;
}

void Client::feed(char *buf)
{
	_msg += buf;
}

void Client::authenticate(string passwd, vector<string> arg)
{
	if (passwd == arg[0])
	{
		sendMsg("authenticated\n");
		_is_authenticated = true;
	}
	else
		sendMsg("wrong passwd\n");
}

void Client::nick(map<string, int>& client_map, string arg)
{
	if (_nickname != "")
		client_map.erase(_nickname);
	_nickname = arg;
	client_map[arg] = _idx;
	if (_is_authenticated && _nickname != "" && _username != "")
	{
		_is_registered = true;
		sendMsg("registered\n");
	}
}

void Client::user(vector<string> args)
{
	_username = args[0];
	if (_is_authenticated && _nickname != "" && _username != "")
	{
		_is_registered = true;
		sendMsg("registered\n");
	}
}

void Client::sendMsg(string message, int flag)
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

int Client::mode(void)
{
	return _mode;
}

void Client::mode_add(int m)
{
	_mode |= m;
	if (m == MODE_i)
		sendMsg("you are invisible\n");
	else if (m == MODE_o)
		sendMsg("you are operator\n");
}

void Client::mode_remove(int m)
{
	_mode &= ~m;
	if (m == MODE_i)
		sendMsg("you are visible\n");
	else if (m == MODE_o)
		sendMsg("you are not operator\n");
}