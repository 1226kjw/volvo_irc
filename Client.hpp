#ifndef CLIENT_HPP
# define CLIENT_HPP

class Client
{
public:
	bool auth;
	string nickname;
	string username;
	string msg;
	int idx;

	Client() : auth(false), nickname(""), username(""), msg(""), idx(-1) {}
	Client(int idx) : auth(false), nickname(""), username(""), msg(""), idx(idx) {}
	Client(const Client& a)
	{
		*this = a;
	}
	Client& operator=(const Client& a)
	{
		auth = a.auth;
		nickname = a.nickname;
		username = a.username;
		msg = a.msg;
		idx = a.idx;
		return *this;
	}
	void cmd(map<string, set<Client*> > &channel, map<string, Client> &client, pollfd *client_fd)
	{
		
	}
};

#endif