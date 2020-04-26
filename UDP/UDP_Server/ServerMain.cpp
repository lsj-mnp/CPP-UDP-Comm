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
				bool receive = server.receive();

				if (receive == true)
				{
					OutputDebugString(TEXT("���ú꿡 �����߽��ϴ�! \n"));
				}

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

		bool send = server.sendToAll(send_message);

		if (send == true)
		{
			OutputDebugString(TEXT("�޼����� ������ �Ϳ� �����߽��ϴ�! \n"));
		}
	}

	thread_receive.join();

	return 0;
}