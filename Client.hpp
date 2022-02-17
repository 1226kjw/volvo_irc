#ifndef CLIENT_HPP
# define CLIENT_HPP

class Client
{
public:
	static set<string> taken;
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
	void authenticate(string passwd, string arg)
	{
		if (is_authenticated)
		{
			send(fd, "already authenticated\n", 23, 0);
			return ;
		}
		while (arg.back() == '\n')
			arg.pop_back();
		if (passwd == arg)
		{
			send(fd, "authenticated\n", 15, 0);
			is_authenticated = true;
		}
		else
		{
			send(fd, "wrong pw\n", 10, 0);
		}
	}
	void nick(map<string, int> &client_map, string arg)
	{
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
	void user(string arg)
	{
		username = arg;
		if (is_authenticated && nickname != "" && username != "")
		{
			is_registered = true;
			send(fd, "registered\n", 12, 0);
		}
	}
};

#endif