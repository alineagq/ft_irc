#include "../CommandHandler.hpp"

void CommandHandler::cmdQuit(const std::string &param, int fd) {
	std::vector<std::string> tokens = split(param, ' ');
	if (tokens.size() < 1) {
		sendMsg(fd, "Usage: QUIT [message]\r\n");
		return;
	}
	std::string msg;
	for (std::vector<std::string>::size_type i = 0; i < tokens.size(); i++) {
		msg += tokens[i];
		if (i + 1 < tokens.size())
			msg += " ";
	}
	std::map<std::string, Channel>::iterator it = m_channels->begin();
	std::string userNick = (*m_users)[fd].getNickname();
	while (it != m_channels->end()) {
		if (it->second.hasUser(userNick)) {
			it->second.removeUser(userNick);
			broadcastChannel(it->first, (*m_users)[fd].getNickname() + " left the channel.\r\n");
		}
		++it;
	}
	sendMsg(fd, "Goodbye!\r\n");
	close(fd);
	m_users->erase(fd);
}