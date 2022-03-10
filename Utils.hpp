#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <map>
# include <set>
# include <vector>
# include <queue>
# include <cstring>

# include <unistd.h>

# include "Client.hpp"
# include "Channel.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::map;
using std::set;
using std::vector;
using std::priority_queue;

vector<string> split(string str, char d=' ');
bool isin(char c, string pool);


#endif