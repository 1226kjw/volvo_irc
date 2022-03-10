#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <set>
# include <map>

# include "Client.hpp"
# include "Utils.hpp"

using std::string;
using std::set;
using std::map;

class Channel
{
private:
	string _name;
	string _manager;
	string _topic;
	set<int> _member;

public:
	Channel();
	Channel(const Channel& c);
	Channel(string name, string manager);
	Channel& operator=(const Channel& c);
	~Channel();

	bool isin(int i);
	void sendMsg(map<int, Client> &client, int i, string msg, int self=1);
	void join(Client &c);
	void out(Client& c);
	string manager();
};

#endif