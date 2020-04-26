#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <mutex>
#include <queue>

class CServer
{
public:
	CServer()
	{
		startup();
		createSocket();
		bindSocket();
		setHostAddr();
	}
	~CServer()
	{
		cleanup();
	}

private:
	bool startup()
	{
		WSADATA wsa_data{};

		int error{ WSAStartup(MAKEWORD(2, 2), &wsa_data) };

		if (error != 0)
		{
			printf("error! code: %d\n", error);

			return false;
		}
		return true;
	}

	bool createSocket()
	{
		m_server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (m_server_socket == INVALID_SOCKET)
		{
			printf("error! code: %d\n", WSAGetLastError());

			return false;
		}

		return true;
	}


	bool cleanup()
	{
		int socket_error = closesocket(m_server_socket);

		if (socket_error == SOCKET_ERROR)
		{
			printf("error! code: %d\n", WSAGetLastError());
		}

		int error{ WSACleanup() };

		if (error != 0)
		{
			printf("error! code: %d\n", error);

			return false;
		}
		return true;
	}

	bool bindSocket()
	{
		sockaddr_in addr{};
		addr.sin_family = AF_INET;

		addr.sin_port = htons(SERVICE_PORT);

		addr.sin_addr.S_un.S_addr = INADDR_ANY;


		int error{ bind(m_server_socket, (sockaddr*)&addr, sizeof(addr)) };


		if (error == SOCKET_ERROR)
		{
			printf("error! code: %d\n", WSAGetLastError());

			return false;
		}

		return true;
	}

	void setHostAddr()
	{
		char buf[256]{};

		if (gethostname(buf, (int)(sizeof(buf) / sizeof(char))) == SOCKET_ERROR)
		{
			printf("error! code: %d\n", WSAGetLastError());

			return;
		}

		ADDRINFOA hint{};
		hint.ai_family = PF_INET;

		ADDRINFOA* result{};

		int error{ getaddrinfo(buf, nullptr, &hint, &result) };

		if (error != 0)
		{
			printf("error! code: %d\n", error);

			return;
		}

		memcpy(&m_host_addr, result->ai_addr, sizeof(m_host_addr));

		char ip[16]{};

		inet_ntop(AF_INET, &m_host_addr.sin_addr, ip, sizeof(ip));

		m_host_ip = ip;

		freeaddrinfo(result);
	}

public:
	bool receive()
	{
		sockaddr_in c_addr{};

		int c_add{ sizeof(c_addr) };

		int byte_count{ recvfrom(m_server_socket, m_buf, m_buffer_size, 0, (sockaddr*)&c_addr, &c_add) };

		if (byte_count > 0)
		{

			for (const auto& client : m_v_c_addrs)
			{
				if (client.sin_addr.S_un.S_addr == c_addr.sin_addr.S_un.S_addr)
				{
					return true;
				}
			}

			m_q_joining_client_addrs.push(c_addr);

			return true;
		}

		return false;
	}

	bool sendToAll(const char* message)
	{
		int len{ (int)strlen(message) };

		bool success{ true };

		while (m_q_joining_client_addrs.size() > 0)
		{
			sockaddr_in jca = m_q_joining_client_addrs.front();

			m_v_c_addrs.emplace_back(jca);

			m_q_joining_client_addrs.pop();
		}

		for (const auto& c_addr : m_v_c_addrs)
		{
			int byte_count = sendto(m_server_socket, message, len, 0, (sockaddr*)&c_addr, sizeof(c_addr));

			if (byte_count <= 0)
			{
				success = false;
			}

			m_buf[(long long)(byte_count - 1)] = 0;
		}

		return success;
	}

public:
	const char* getBuf() const
	{
		return m_buf;
	}

	const std::string& getHostIp() const
	{
		return m_host_ip;
	}

	unsigned short getServicePort() const
	{
		return SERVICE_PORT;
	}

private:
	static const int m_buffer_size{ 1024 };
	const unsigned short SERVICE_PORT{ 15000 };



private:
	SOCKET m_server_socket{};
	char m_buf[m_buffer_size]{};
	sockaddr_in m_host_addr{};
	std::string m_host_ip{};

private:
	std::mutex m_mtx{};

private:
	std::vector<sockaddr_in> m_v_c_addrs{};
	std::queue<sockaddr_in> m_q_joining_client_addrs{};
};
