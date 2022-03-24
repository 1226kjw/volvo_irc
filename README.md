# 💬 ft_irc


## 📝 Description
This project is about creating your **own IRC server**.

You will use an actual IRC client to connect to your server and test it.

Internet is ruled by solid standards protocols that allow connected computers to interact with each other.


## ⚙️ Functions

| Command       | Description |
| ------------- | ----------- |
| PASS          | Set a 'connection password'.       |
| NICK          | Give user a nickname or change the existing one.        |
| USER          | Beginning of connection to specify the username, hostname and realname of a new user.       |
| OPER          | Obtain operator privileges.        |
| MODE          | Changes which affect either how the client is seen by others or what 'extra' messages the client is sent.       |
| JOIN          | Request to start listening to the specific channel.        |
| KICK          | Request the forced removal of a user from a channel.        |
| PART          | Sending the message to be removed from the list of active members for all given channels listed in the parameter string.        |
| NAMES         | Can list all nicknames that are visible to him.        |
| PRIVMSG       | Send private messages between users, as well as to send messages to channels.|
| NOTICE        | similarly to PRIVMSG. The difference between NOTICE and PRIVMSG is that automatic replies MUST NEVER be sent in response to a NOTICE message.        |
| QUIT          | Terminated with a quit message.        |

- Based on [RFC 2812](https://datatracker.ietf.org/doc/html/rfc2812).
- Test using [Hexchat](https://github.com/hexchat/hexchat).


## 🚘 Team(Volvo XC90)

- [Jihae Kim](https://github.com/deftones88)
- [Yeshin Lee](https://github.com/yeslee-v)
- [Jinuk Kim](https://github.com/1226kjw)
