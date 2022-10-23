#include "Server.hpp"

namespace cow
{
	namespace tcp
	{
		Client::Client(SOCKET listening_socket)
		{
			m_clientSocket = socket(AF_INET, SOCK_STREAM, 0);

			sockaddr_in socket;
			int socketSize = sizeof(socket);

			m_clientSocket = accept(listening_socket, (sockaddr*)&socket, &socketSize);
			if (m_clientSocket == INVALID_SOCKET)
			{
				std::cerr << "\033[31mClient Socket Appears to be Invalid\033[0m\n";
				ExitProcess(1);
			}
			memset(m_host, 0, NI_MAXHOST);
			memset(m_port, 0, NI_MAXSERV);
		}
		Client::Client(const char* ipAdress, unsigned int port)
		{
			m_clientSocket = socket(AF_INET, SOCK_STREAM, 0);

			sockaddr_in hint{};
			hint.sin_family = AF_INET;
			hint.sin_port = htons(port);
			inet_pton(AF_INET, ipAdress, &hint.sin_addr);
			int connResult = connect(m_clientSocket, (sockaddr*)&hint, sizeof(hint));
			if (connResult == SOCKET_ERROR)
			{
				std::cerr << "\033[31mwhile connecting to client socket an exception was thrown\033[0m\n";
				closesocket(m_clientSocket);
				WSACleanup();
				ExitProcess(1);
			}
			memset(m_host, 0, NI_MAXHOST);
			memset(m_port, 0, NI_MAXSERV);

			getnameinfo((sockaddr*)&m_clientSocket, sizeof(m_clientSocket), m_host, NI_MAXHOST, m_port, NI_MAXSERV, 0);
		}

		Client::~Client()
		{
			closesocket(m_clientSocket);
		}

		// Listener
		Listener::Listener(int protocol, int port)
		{
			m_listening = socket(AF_INET, SOCK_STREAM, protocol);

			if (m_listening == INVALID_SOCKET)
			{
				std::cerr << "Can't create socket\n";
				ExitProcess(1);
			}
			sockaddr_in hint{};
			hint.sin_family = AF_INET;
			hint.sin_port = htons(port);
			hint.sin_addr.S_un.S_addr = INADDR_ANY; // ? Could also use inet_pton to select specific Ip Address ...
			int result = bind(m_listening, (sockaddr*)&hint, sizeof(hint));
			if (result == SOCKET_ERROR)
			{
				std::cout << WSAGetLastError() << '\n';
			}
		}

		Listener::~Listener()
		{
			closesocket(m_listening);
		}
		void Listener::remakeConnection(int protocol, int port)
		{
			if (m_listening != 0)
				closesocket(m_listening);	// Close existing socket

			m_listening = socket(AF_INET, SOCK_STREAM, protocol);
			
			sockaddr_in hint{};
			hint.sin_family = AF_INET;
			hint.sin_port = htons(port);
			hint.sin_addr.S_un.S_addr = INADDR_ANY; // ? Could also use inet_pton to select specific Ip Address ...
			int result = bind(m_listening, (sockaddr*)&hint, sizeof(hint));
			if (result == SOCKET_ERROR)
			{
				std::cout << WSAGetLastError() << '\n';
			}
		}
	}

	namespace udp
	{
	}
};
