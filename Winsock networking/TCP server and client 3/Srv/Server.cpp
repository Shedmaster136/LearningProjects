/*
 * Server.cpp
 *
 *  Created on: 13 окт. 2020 г.
 *      Author: Steep
 */




#include "WinSockWrapper.h"
#include <iostream>

const std::string endLine = "\n";

inline void SocketErrorHandler(std::string errMsg, int errNum, TCP_Socket_Wrapper errSocket)
{
	std::cout << errMsg << " " << errNum <<endLine;
	errSocket.CloseSocket();
	TCP_Socket_Wrapper::CloseSocketAPI();
}

int main(int argc, char *argv[])
{
	//получить аргументы из командной строки (1 - порт сервера
	// 2 - IP
	int errNum = 0;

	if(argc != 3)
	{
		std::cout << "Неверное количество аргументов" << endLine;
		return -1;
	}
	int serverPort = 0;
	serverPort = std::stoi(std::string(argv[1]));
	if(serverPort == 0)
	{
		std::cout << "Неверный порт" << endLine;
		return -1;
	}
	std::string serverAddress(argv[2]);

	if((errNum = TCP_Socket_Wrapper::InitSocketAPI()))
	{
		std::cout << "Ошибка инициализации "<< errNum << endLine;
		return -1;
	}

	TCP_Socket_Wrapper serverSocket(serverPort, serverAddress);
	if((errNum = serverSocket.CreateSocket()))
	{
		SocketErrorHandler("Ошибка создания сокета", errNum, serverSocket);
		return -1;
	}
	if((errNum = serverSocket.BindSocket()))
	{
		SocketErrorHandler("Ошибка привязки сокета", errNum, serverSocket);
		return -1;
	}

	if((errNum = serverSocket.ListenSocket(2)))
	{
		SocketErrorHandler("Ошибка прослушивания подключений", errNum, serverSocket);
		return -1;
	}

	TCP_Socket_Wrapper clientSocket;
	std::string rcvBuf = "";
	while(true)
	{
		if((errNum = serverSocket.AcceptConnection(clientSocket)))
		{

			SocketErrorHandler("Ошибка соединения с клиентом", errNum, serverSocket);
			return -1;
		}
		do{
			if((errNum = clientSocket.RecievePacket(rcvBuf)))
			{
				SocketErrorHandler("Ошибка получения сообщения", errNum, clientSocket);
				continue;
			}
			std::cout << rcvBuf << endLine;
			if((errNum = clientSocket.SendPacket("Server Answer")))
			{
				SocketErrorHandler("Ошибка получения сообщения", errNum, clientSocket);
				continue;
			}
		}while(rcvBuf != "exit");
		clientSocket.CloseSocket();
	}

	return 0;
}
