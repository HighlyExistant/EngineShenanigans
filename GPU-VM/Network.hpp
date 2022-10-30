
#ifndef COWSERVER1
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <WS2tcpip.h>   // Winsock Header File
#include <Windows.h>
#include <mswsock.h>
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "mswsock.lib")
#include <stdexcept>
#include <vector>
namespace cow 
{
	namespace tcp 
	{
		class Client
		{
		public:
			Client(SOCKET listener);
			Client(const char* ipAdress, unsigned int port);
			~Client();

			inline void out(const char *buf, int size, int flags = 0)
			{
				send(m_socket, buf, size, flags);
			}
			inline void out(const char* buf)
			{
				send(m_socket, buf, strlen(buf), 0);
			}
			inline int in(char* buf, int size, int flags = 0)
			{
				return recv(m_socket, buf, size, flags);
			}
			void block_state(u_long* pMode);
		protected:
			SOCKET m_socket;
		};

		class Listener
		{
		public:
			Listener(int protocol, int port);
			~Listener();

			inline int start() { return listen(m_socket, SOMAXCONN); }
			inline Client accept() { return Client{ m_socket }; }
			inline Client* acceptDyn() { return new Client{ m_socket }; }
		private:
			SOCKET m_socket;
		};
	}
	namespace udp 
	{
		class Client
		{
		public:
			~Client();
			static inline Client createClient(const char* pIpAddress, int port)
			{
				return Client(pIpAddress, port);
			}
			static inline Client createServer(const char* pIpAddress, int port)
			{
				return Client(port, pIpAddress);
			}
			int in(char* buf, int size, int flags = 0)
			{
				int addr_size = sizeof(client_addr);
				return recvfrom(m_socket, buf, size, flags, (sockaddr*)&client_addr, &addr_size);
			}
			void out(const char* buf, int size, int flags = 0)
			{
				sendto(m_socket, buf, size, flags, (sockaddr*)&client_addr, sizeof(client_addr));
			}
			void out(char* buf, int size, int flags = 0)
			{
				sendto(m_socket, buf, size, flags, (sockaddr*)&client_addr, sizeof(client_addr));
			}
		protected:
			SOCKET m_socket;
			sockaddr_in client_addr;
		private:
			// Inverted this stuff to seperate them cause
			// C++ sucks at polymorphism with templates
			// apparently
			Client(int port, const char* pIpAddress);
			Client(const char* pIpAddress, int port);
		};
	}
	namespace net
	{
		enum class ServerFlags
		{
			DEFAULT = 0,
			GLOBAL = 1,
			EXCLUDE = 2
		};

		class TcpServer
		{
		public:
			TcpServer(int port)
				: listener{ 0, port }
			{

			}
			~TcpServer()
			{
				int size = clients.size();
				for (size_t i = 0; i < size; i++)
				{
					delete clients[i];
				}
			}
			void listen()
			{
				listener.start();
				clients.push_back(listener.acceptDyn());
			}

			void write(const char* str, ServerFlags flags, int index = 0)
			{
				if (flags == ServerFlags::GLOBAL)
				{
					for (size_t i = 0; i < clients.size(); i++)
					{
						if (clients[i] != nullptr)
							clients[i]->out(str);
					}
					return;
				}
				if (flags == ServerFlags::EXCLUDE)
				{
					for (size_t i = 0; i < clients.size(); i++)
					{
						if (index == i) continue;
						if (clients[i] != nullptr)
							clients[i]->out(str);
					}
					return;
				}
			}
			void write(const char* str, int index)
			{
				clients[index]->out(str);
			}
			inline size_t clientCount()
			{
				return clients.size();
			}
		private:
			cow::tcp::Listener listener;
			std::vector<cow::tcp::Client*> clients;	// change this for an unordered_map
		};
	}
}
#endif