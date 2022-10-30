#include "Network.hpp"

#ifndef COWSERVER1
#define COWSERVER1

namespace cow 
{
	namespace tcp 
	{
		Client::Client(SOCKET listener)
		{
			m_socket = socket(AF_INET, SOCK_STREAM, 0);

			sockaddr_in sock_in;
			int socketSize = sizeof(sock_in);

			m_socket = accept(listener, (sockaddr*)&sock_in, &socketSize);
			if (m_socket == INVALID_SOCKET)
			{
				std::cerr << "\033[31mclient socket appears to be invalid\033[0m\n";
				throw std::runtime_error("Client Socket Appears to be Invalid");
			}
		}
		Client::Client(const char* ipAdress, unsigned int port)
			: m_socket{ socket(AF_INET, SOCK_STREAM, 0) }
		{
			sockaddr_in sock_in{};
			sock_in.sin_family = AF_INET;
			sock_in.sin_port = htons(port);
			inet_pton(AF_INET, ipAdress, &sock_in.sin_addr);
			if (connect(m_socket, (sockaddr*)&sock_in, sizeof(sock_in)) == SOCKET_ERROR)
			{
				std::cerr << "\033[31mwhile connecting to client socket an exception was thrown\033[0m\n";
				closesocket(m_socket);
				throw std::runtime_error("while connecting to client socket an exception was thrown");
			}
		}
		Client::~Client()
		{
			closesocket(m_socket);
		}

		void Client::block_state(u_long* pMode)
		{
			int iResult = ioctlsocket(m_socket, FIONBIO, pMode);
			if (iResult != NO_ERROR)
			{
				printf("\033[31mfailed to change blocking state\033[0m\n");
			}
		}

		Listener::Listener(int protocol, int port)
		{
			m_socket = socket(AF_INET, SOCK_STREAM, protocol);

			if (m_socket == INVALID_SOCKET)
			{
				std::cerr << "Can't create socket\n";
				ExitProcess(1);
			}
			sockaddr_in hint{};
			hint.sin_family = AF_INET;
			hint.sin_port = htons(port);
			hint.sin_addr.S_un.S_addr = INADDR_ANY; // ? Could also use inet_pton to select specific Ip Address ...
			int result = bind(m_socket, (sockaddr*)&hint, sizeof(hint));
			if (result == SOCKET_ERROR)
			{
				std::cout << WSAGetLastError() << '\n';
			}
		}

		Listener::~Listener()
		{
			closesocket(m_socket);
		}
	}
	namespace udp 
	{
		Client::Client(int port, const char* pIpAddress)
			: m_socket{ socket(AF_INET, SOCK_DGRAM, 0) }
		{
			if (m_socket == SOCKET_ERROR)
			{
				std::cerr << "\033[31mclient socket appears to be invalid\033[0m\n";
				WSACleanup();
				throw std::runtime_error("client socket appears to be invalid");
			}
			sockaddr_in server_addr;
			memset(&server_addr, 0, sizeof(server_addr));
			memset(&client_addr, 0, sizeof(client_addr));

			server_addr.sin_family = AF_INET;
			server_addr.sin_port = htons(port);
			inet_pton(AF_INET, pIpAddress, &server_addr.sin_addr);
			int n = bind(m_socket, (sockaddr*)&server_addr, sizeof(server_addr));

			if (n == SOCKET_ERROR)
			{
				std::cerr << "\033[31mwhile connecting to client socket an exception was thrown\033[0m\n";
				closesocket(m_socket);
				WSACleanup();
				ExitProcess(1);
			}
		}
		Client::Client(const char* pIpAddress, int port)
			: m_socket{ socket(AF_INET, SOCK_DGRAM, 0) }
		{
			if (m_socket == SOCKET_ERROR)
			{
				std::cerr << "\033[31mClient Socket Appears to be Invalid\033[0m\n";
				WSACleanup();
				ExitProcess(1);
			}

			client_addr.sin_family = AF_INET;
			client_addr.sin_port = htons(port);
			inet_pton(AF_INET, pIpAddress, &client_addr.sin_addr);
		}
		Client::~Client()
		{
			closesocket(m_socket);
		}
	}

}
#endif