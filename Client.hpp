#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>
# include <map>
# include <set>

# include <sys/socket.h>

using std::string;
using std::vector;
using std::map;
using std::set;

enum clientState
{
	VALID,
	WRONG_PW,
	NICKNAMEINUSE,
	NEEDMOREPARAMS,
	UNAUTHENTICATED,
	ALREADY_AUTHENTICATED,
	NOT_REGISTERED
};

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
	Client();
	Client(int idx, int fd);
	Client(const Client& a);
	~Client();
	Client& operator=(const Client& a);
	void feed(char *buf);
	void authenticate(string passwd, vector<string> arg);
	void nick(map<string, int>& client_map, string arg);
	void user(vector<string> args);
	void sendMsg(string message, int flag=0);
	string prefix(void);

	bool is_registered(void);
	bool is_authenticated(void);
	string message(void);
	void message(string s);
	set<string>& joined_channel(void);
	string nickname(void);
	string username(void);
	int  idx(void);
	int  fd(void);
};

#endif