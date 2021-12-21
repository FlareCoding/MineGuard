#include "control_server.h"

control_server::control_server()
{
	m_Socket = CreateSocket();
}

void control_server::wait_for_connection()
{
	m_Socket->Bind("0.0.0.0", SERVER_PORT);
	m_Socket->Listen(1);
	m_Socket->Accept();
}

void control_server::close_connection()
{
	m_Socket->Close();
}

bool control_server::send(const std::string& str)
{
	size_t result = m_Socket->Send(str);
	return result > 0;
}

std::pair<size_t, std::string> control_server::receive()
{
	size_t bytes;
	std::string data = m_Socket->Recv(4096, &bytes);

	return { bytes, data };
}

void control_server::start_communication_loop(std::function<void(const std::string&, std::string&)> handler)
{
	while (true)
	{
		auto [bytes, cmd] = receive();
		if (!bytes)
			break;

		std::string response;
		handler(cmd, response);

		bool result = send(response);

		if (!result)
			break;
	}
}

control_server::~control_server()
{
	close_connection();
}
