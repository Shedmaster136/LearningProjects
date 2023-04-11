/*
 * Server.cpp
 *
 *  Created on: 13 ���. 2020 �.
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
	//�������� ��������� �� ��������� ������ (1 - ���� �������
	// 2 - IP
	int errNum = 0;

	if(argc != 3)
	{
		std::cout << "�������� ���������� ����������" << endLine;
		return -1;
	}
	int serverPort = 0;
	serverPort = std::stoi(std::string(argv[1]));
	if(serverPort == 0)
	{
		std::cout << "�������� ����" << endLine;
		return -1;
	}
	std::string serverAddress(argv[2]);

	if((errNum = TCP_Socket_Wrapper::InitSocketAPI()))
	{
		std::cout << "������ ������������� "<< errNum << endLine;
		return -1;
	}

	TCP_Socket_Wrapper serverSocket(serverPort, serverAddress);
	if((errNum = serverSocket.CreateSocket()))
	{
		SocketErrorHandler("������ �������� ������", errNum, serverSocket);
		return -1;
	}
	if((errNum = serverSocket.BindSocket()))
	{
		SocketErrorHandler("������ �������� ������", errNum, serverSocket);
		return -1;
	}

	if((errNum = serverSocket.ListenSocket(2)))
	{
		SocketErrorHandler("������ ������������� �����������", errNum, serverSocket);
		return -1;
	}

	TCP_Socket_Wrapper clientSocket;
	std::string rcvBuf = "";
	while(true)
	{
		if((errNum = serverSocket.AcceptConnection(clientSocket)))
		{

			SocketErrorHandler("������ ���������� � ��������", errNum, serverSocket);
			return -1;
		}
		do{
			if((errNum = clientSocket.RecievePacket(rcvBuf)))
			{
				SocketErrorHandler("������ ��������� ���������", errNum, clientSocket);
				continue;
			}
			std::cout << rcvBuf << endLine;
			if((errNum = clientSocket.SendPacket("Server Answer")))
			{
				SocketErrorHandler("������ ��������� ���������", errNum, clientSocket);
				continue;
			}
		}while(rcvBuf != "exit");
		clientSocket.CloseSocket();
	}

	return 0;
}
