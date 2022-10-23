#pragma once

#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <WS2tcpip.h>   // Winsock Header File
#include <Windows.h>
#include <mswsock.h>
#include <string>
#include <vector>
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "mswsock.lib")
namespace cow
{
	namespace tcp
	{
		class Client
		{
		public:
			Client(SOCKET listening_socket);
			Client(const char* ipAdress, unsigned int port);
			~Client();

			inline void write(const char* str, int length, int flags = 0)
			{
				send(m_clientSocket, str, length, flags);
			}
			inline void write(const char* str)
			{
				send(m_clientSocket, str, strlen(str), 0);
			}
			inline int recieve(char* buf, int size, int flags = 0)
			{
				return recv(m_clientSocket, (char*)buf, size, flags);
			}
		private:
			SOCKET m_clientSocket;
			sockaddr_in client;

			char m_host[NI_MAXHOST];
			char m_port[NI_MAXSERV];
		};

		class Listener
		{
		public:
			Listener(int protocol, int port);
			~Listener();

			void remakeConnection(int protocol, int port);
			inline int startListen() { return listen(m_listening, SOMAXCONN); }
			inline Client acceptClient() { return Client{ m_listening }; }
			Client *acceptClientDynamic() { return new Client{ m_listening }; }

		private:
			SOCKET m_listening;
		};
	}
	namespace udp 
	{
		class Server
		{
		public:
			Server(const char* pIpAddress, int port)
				: m_clientSocket{ socket(AF_INET, SOCK_DGRAM, 0) }
			{
				if (m_clientSocket == SOCKET_ERROR)
				{
					std::cerr << "\033[31mClient Socket Appears to be Invalid\033[0m\n";
					WSACleanup();
					ExitProcess(1);
				}
				memset(&server_addr, 0, sizeof(server_addr));
				memset(&client_addr, 0, sizeof(client_addr));

				server_addr.sin_family = AF_INET;
				server_addr.sin_port = htons(54000);
				inet_pton(AF_INET, pIpAddress, &server_addr.sin_addr);
				int n = bind(m_clientSocket, (sockaddr*)&server_addr, sizeof(server_addr));

				if (n == SOCKET_ERROR) 
				{
					std::cerr << "\033[31mwhile connecting to client socket an exception was thrown\033[0m\n";
					closesocket(m_clientSocket);
					WSACleanup();
					ExitProcess(1);
				}
			}
			~Server()
			{
				closesocket(m_clientSocket);
			}
			void recieve(char* buf, int size, int flags = 0) 
			{
				int addr_size = sizeof(client_addr);
				recvfrom(m_clientSocket, buf, size, flags, (sockaddr*)&client_addr, &addr_size);
			}
			void write(char* buf, int size, int flags = 0)
			{
				sendto(m_clientSocket, buf, size, flags, (sockaddr*)&client_addr, sizeof(client_addr));
			}
		private:
			SOCKET m_clientSocket;
			sockaddr_in server_addr, client_addr;
		};
		class Client
		{
		public:
			Client(const char* pIpAddress, int port)
				: m_clientSocket{ socket(AF_INET, SOCK_DGRAM, 0) }
			{
				if (m_clientSocket == SOCKET_ERROR)
				{
					std::cerr << "\033[31mClient Socket Appears to be Invalid\033[0m\n";
					WSACleanup();
					ExitProcess(1);
				}

				client_addr.sin_family = AF_INET;
				client_addr.sin_port = htons(port);
				inet_pton(AF_INET, pIpAddress, &client_addr.sin_addr);
			}
			~Client() 
			{
				closesocket(m_clientSocket);
			}
			void write(char* buf, int size, int flags = 0)
			{
				sendto(m_clientSocket, buf, size, flags, (sockaddr*)&client_addr, sizeof(client_addr));
			}
			void recieve(char* buf, int size, int flags = 0)
			{
				int addr_size = sizeof(client_addr);
				recvfrom(m_clientSocket, buf, size, flags, (sockaddr*)&client_addr, &addr_size);
			}
		private:
			SOCKET m_clientSocket;
			sockaddr_in client_addr;
		};
	}
};
