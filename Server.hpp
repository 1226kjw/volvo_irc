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

#define CLIENT_MAX 1000
#define BUFFER_SIZE 1024
#define CHANNEL_PREFIX "#&+!"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::map;
using std::set;
using std::vector;
using std::priority_queue;

class Server
{
public:
	const int port;
	const string passwd;
	map<string, int> client_map;
	map<string, Channel> channel;
	map<int, Client> client;
	struct pollfd client_fd[CLIENT_MAX];
	priority_queue<int> available_index;

	struct sockaddr_in server_addr, client_addr;
	socklen_t clientaddr_len;
	int server_socket;
	char buf[BUFFER_SIZE + 1];

	int max_index;
	int idx;

	Server(int port, string pw);
	~Server();
	int setup();
	int run();
	void cmd(int i);
	void enroll(int i, string command, vector<string> arg);
	void join(int i, vector<string> arg);
	void part(int i, vector<string> arg);
	void list(int i, vector<string> arg);
	void kick(int i, vector<string> arg);
	void privmsg(int i, vector<string> arg);
	void quit(int i, vector<string> arg);
};

#endif