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
	const string oper_name = "kim";
	const string oper_pw = "lee";

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

	class ERR_WRONG_PW : public std::exception
	{
	public:
		virtual const char* what() const throw();
	};

	class ERR_NICKNAMEINUSE : public std::exception
	{
	public:
		virtual const char* what() const throw();
	};
	
	class ERR_NEEDMOREPARAMS : public std::exception
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
};

#endif