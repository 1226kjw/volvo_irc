#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <vector>
# include <iostream>
# include <stdexcept>

std::vector<std::string> split(std::string str, char d=' ');
bool isin(char c, std::string pool);
bool nickcheck(std::string str);
int irc_atoi(std::string str);

#endif
