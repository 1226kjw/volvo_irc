#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <vector>
# include <queue>
# include <cstring>

# include <unistd.h>

using std::string;
using std::vector;

vector<string> split(string str, char d=' ');
bool isin(char c, string pool);


#endif