#ifndef CLIENT_HPP
# define CLIENT_HPP

class Client
{
public:
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
			cout << "already authenticated" << endl;
			return ;
		}
		while (arg.back() == '\n')
			arg.pop_back();
		if (passwd == arg)
		{
			cout << "authenticated." << endl;
			is_authenticated = true;
		}
		else
		{
			cout << "wrong pw." << endl;
		}
	}
	void nick(string arg)
	{

	}
	void user(string arg)
	{
		
	}
};

#endif