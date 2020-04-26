#include "CClient.h"

int main()
{
	CClient client{};

	client.SetServerAddr("192.168.35.60", 15000);

	std::thread thrReceive
	{
		[&]
		{
			while (true)
			{
				if (client.Receive())
				{
					printf("%s\n", client.GetBuff());
				}
			}
		}
	};

	char sendMessage[5000]{};

	while (true)
	{
		fgets(sendMessage, 5000, stdin);

		client.Send(sendMessage);
	}

	thrReceive.join();

	return 0;
}