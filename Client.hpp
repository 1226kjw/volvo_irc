#ifndef CLIENT_HPP
# define CLIENT_HPP

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::map;
using std::set;
using std::vector;
using std::priority_queue;

class Client
{
private:
	set<string> _joined_channel;
	bool _is_authenticated;
	bool _is_registered;
	string _nickname;
	string _username;
	string _msg;
	int _idx;
	int _fd;

public:
	Client() : _is_authenticated(false), _is_registered(false), _nickname(""), _username(""), _msg(""), _idx(-1), _fd(-1) {}
	Client(int idx, int fd) : _is_authenticated(false), _is_registered(false), _nickname(""), _username(""), _msg(""), _idx(idx), _fd(fd) {}
	Client(const Client& a)
	{
		*this = a;
	}
	Client& operator=(const Client& a)
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
	void feed(char *buf)
	{
		_msg += buf;
	}
	void authenticate(string passwd, vector<string> arg)
	{
		if (arg.size() == 1 && passwd == arg[0])
		{
			sendMsg("authenticated\n");
			_is_authenticated = true;
		}
		else
		{
			sendMsg("wrong pw\n");
		}
	}
	void nick(map<string, int> &client_map, vector<string> args)
	{
		if (args.size() != 1)
		{
			sendMsg("invalid num of args\n");
		}
		string arg = args[0];
		if (client_map.find(arg) != client_map.end())
		{
			sendMsg("already taken.\n");
			return ;
		}
		_nickname = arg;
		client_map[arg] = _idx;
		if (_is_authenticated && _nickname != "" && _username != "")
		{
			_is_registered = true;
			sendMsg("registered\n");
		}
	}
	void user(vector<string> args)
	{
		if (args.size() != 1)
		{
			sendMsg("invalid num of args\n");
			return ;
		}
		_username = args[0];
		if (_is_authenticated && _nickname != "" && _username != "")
		{
			_is_registered = true;
			sendMsg("registered\n");
		}
	}
	void sendMsg(string message, int flag=0)
	{
		send(_fd, message.c_str(), message.size(), flag);
	}
	string prefix(void)
	{
		return string(":") + _username + ' ';
	}

	bool is_registered(void)
	{
		return _is_registered;
	}
	bool is_authenticated(void)
	{
		return _is_authenticated;
	}
	string message(void)
	{
		return _msg;
	}
	void message(string s)
	{
		_msg = s;
	}
	set<string>& joined_channel(void)
	{
		return _joined_channel;
	}
	string nickname(void)
	{
		return _nickname;
	}
	string username(void)
	{
		return _username;
	}
	int  idx(void)
	{
		return _idx;
	}
	int  fd(void)
	{
		return _fd;
	}
};

#endif