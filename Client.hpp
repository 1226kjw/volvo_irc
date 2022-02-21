#ifndef CLIENT_HPP
# define CLIENT_HPP

class Client
{
public:
	set<string> joined_channel;
	bool is_authenticated;
	bool is_registered;
	string nickname;
	string username;
	string msg;
	int idx;
	int fd;

	Client() : is_authenticated(false), is_registered(false), nickname(""), username(""), msg(""), idx(-1), fd(-1) {}
	Client(int idx, int fd) : is_authenticated(false), is_registered(false), nickname(""), username(""), msg(""), idx(idx), fd(fd) {}
	Client(const Client& a)
	{
		*this = a;
	}
	Client& operator=(const Client& a)
	{
		is_authenticated = a.is_authenticated;
		is_registered = a.is_registered;
		nickname = a.nickname;
		username = a.username;
		msg = a.msg;
		idx = a.idx;
		fd = a.fd;
		return *this;
	}
	void feed(char *buf)
	{
		msg += buf;
	}
	void authenticate(string passwd, vector<string> arg)
	{
		if (arg.size() == 1 && passwd == arg[0])
		{
			send(fd, "authenticated\n", 15, 0);
			is_authenticated = true;
		}
		else
		{
			send(fd, "wrong pw\n", 10, 0);
		}
	}
	void nick(map<string, int> &client_map, vector<string> args)
	{
		if (args.size() != 1)
		{
			send(fd, "invalid num of args\n", 21, 0);
		}
		string arg = args[0];
		if (client_map.find(arg) != client_map.end())
		{
			send(fd, "already taken.\n", 16, 0);
			return ;
		}
		nickname = arg;
		client_map[arg] = idx;
		if (is_authenticated && nickname != "" && username != "")
		{
			is_registered = true;
			send(fd, "registered\n", 12, 0);
		}
	}
	void user(vector<string> args)
	{
		if (args.size() != 1)
		{
			send(fd, "invalid num of args\n", 21, 0);
		}
		username = args[0];
		if (is_authenticated && nickname != "" && username != "")
		{
			is_registered = true;
			send(fd, "registered\n", 12, 0);
		}
	}
};

#endif