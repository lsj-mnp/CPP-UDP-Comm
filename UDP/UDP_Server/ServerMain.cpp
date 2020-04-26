#include "CServer.h"
#include <iostream>

int main()
{
	CServer server{};

	std::thread thread_receive
	{
		[&]
		{
			while (true)
			{
				server.receive();

				const char* message = server.getBuf();

				printf(message);
			}
		}
	};

	printf("Server Address: %s:%d \n", server.getHostIp().c_str(), server.getServicePort());

	char send_message[100]{};

	while (true)
	{
		fgets(send_message, 100, stdin);

		server.sendToAll(send_message);
	}

	thread_receive.join();

	return 0;
}