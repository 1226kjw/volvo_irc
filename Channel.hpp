#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <set>
# include <map>

# include "Client.hpp"
# include "Utils.hpp"

class Channel
{
private:
	std::string _name;
	std::string _topic;
	std::set<int> _member;

public:
	Channel();
	Channel(const Channel& c);
	Channel(std::string name);
	Channel& operator=(const Channel& c);
	~Channel();

	bool isin(int i);
	void sendMsg(std::map<int, Client> &client, int i, std::string msg, int self=1);
	void join(Client &c);
	void out(Client& c, std::map<std::string, Channel>& channel);
	std::set<int>& member(void);
};

#endif
