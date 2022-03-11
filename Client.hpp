#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>
# include <map>
# include <set>

# include <sys/socket.h>

class Client
{
private:
	std::set<std::string> _joined_channel;
	bool _is_authenticated;
	bool _is_registered;
	std::string _nickname;
	std::string _username;
	std::string _msg;
	int _idx;
	int _fd;

public:
	Client();
	Client(int idx, int fd);
	Client(const Client& a);
	~Client();
	Client& operator=(const Client& a);
	void feed(char *buf);
	void authenticate(std::string passwd, std::vector<std::string> arg);
	void nick(std::map<std::string, int>& client_map, std::string arg);
	void user(std::vector<std::string> args);
	void sendMsg(std::string message, int flag=0);
	std::string prefix(void);

	bool is_registered(void);
	bool is_authenticated(void);
	std::string message(void);
	void message(std::string s);
	std::set<std::string>& joined_channel(void);
	std::string nickname(void);
	std::string username(void);
	int  idx(void);
	int  fd(void);
};

#endif