#pragma once
#include <map> // for std::pair
#include <functional>
#include "CSFSocket/CSFSocket.h"

#define SERVER_PORT 4554

class control_server
{
public:
	control_server();
	~control_server();

	void wait_for_connection();
	void close_connection();

	bool send(const std::string& str);
	std::pair<size_t, std::string> receive();

	void start_communication_loop(std::function<void(const std::string&, std::string&)> handler);

private:
	std::shared_ptr<CSFSocket> m_Socket = nullptr;
};
