#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <map>
# include <set>
# include <vector>
# include <queue>
# include <cstring>

# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <poll.h>
# include <unistd.h>

# include "Client.hpp"
# include "Channel.hpp"
# include "Utils.hpp"

#define CLIENT_MAX 1000
#define BUFFER_SIZE 1024
#define CHANNEL_PREFIX "#&+!"

class Server
{
private:
	const int port;
	const std::string passwd;
	std::map<std::string, int> client_map;
	std::map<std::string, Channel> channel;
	std::map<int, Client> client;
	struct pollfd client_fd[CLIENT_MAX];
	std::priority_queue<int> available_index;

	struct sockaddr_in server_addr, client_addr;
	socklen_t clientaddr_len;
	int server_socket;
	char buf[BUFFER_SIZE + 1];

	int max_index;
	int idx;
	const std::string oper_name;
	const std::string oper_pw;

public:

	Server(int port, std::string pw);
	~Server();

	int setup();
	int run();
	void cmd(int i);
	void pass(int i, std::vector<std::string> arg);
	void nick(int i, std::vector<std::string> arg);
	void user(int i, std::vector<std::string> arg);
	void oper(int i, std::vector<std::string> arg);
	void mode(int i, std::vector<std::string> arg);
	void join(int i, std::vector<std::string> arg);
	void part(int i, std::vector<std::string> arg);
	void kick(int i, std::vector<std::string> arg);
	void privmsg(int i, std::vector<std::string> arg);
	void quit(int i);
	
	// custom
	class ERR_WRONG_PW : public std::exception
	{
	public:
		virtual const char* what() const throw();
	};

	class ERR_UNAUTHENTICATED : public std::exception
	{
	public:
		virtual const char* what() const throw();
	};
	
	class ERR_ALREADY_AUTHENTICATED : public std::exception
	{
	public:
		virtual const char* what() const throw();
	};

	class ERR_NOT_REGISTERED : public std::exception
	{
	public:
		virtual const char* what() const throw();
	};

	class ERR_NOSUCHUSER : public std::exception
	{
	public:
		virtual const char* what() const throw();
	};

	// several
	class ERR_NEEDMOREPARAMS : public std::exception
	{
	public:
		virtual const char* what() const throw();
	};

	class ERR_ERRONEUSNICKNAME : public std::exception
	{
	public:
		virtual const char *what() const throw();
	};

	class ERR_ALREADYREGISTRED : public std::exception
	{
	public:
		virtual const char* what() const throw();
	};

	class ERR_NOSUCHCHANNEL : public std::exception
	{
	public:
		virtual const char* what() const throw();
	};

	class ERR_USERNOTINCHANNEL : public std::exception
	{
	public:
		virtual const char* what() const throw();
	};

	class ERR_CHANOPRIVSNEEDED : public std::exception
	{
	public:
		virtual const char* what() const throw();
	};

	class ERR_NOSUCHNICK : public std::exception
	{
	public:
		virtual const char* what() const throw();
	};

	// NICK
	class ERR_NICKNAMEINUSE : public std::exception
	{
	public:
		virtual const char* what() const throw();
	};

	// OPER
	class ERR_NOOPERHOST : public std::exception
	{
	public:
		virtual const char *what() const throw();
	};
	
	class ERR_PASSWDMISMATCH : public std::exception
	{
	public:
		virtual const char* what() const throw();
	};


	// MODE
	class ERR_USERSDONTMATCH : public std::exception
	{
	public:
		virtual const char* what() const throw();
	};

	class ERR_UMODEUNKNOWNFLAG : public std::exception
	{
	public:
		virtual const char* what() const throw();
	};

	// PRIVMSG
	class ERR_CANNOTSENDTOCHAN : public std::exception
	{
	public:
		virtual const char* what() const throw();
	};
};

#endif
