/*
 * Client.cpp
 *
 *  Created on: 13 ���. 2020 �.
 *      Author: Steep
 */

#include "WinSockWrapper.h"
#include <iostream>

const std::string endLine = "\n";


void SocketErrorHandler(std::string errMsg, int errNum, TCP_Socket_Wrapper errSocket)
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
	std::string msgData = "";
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

	TCP_Socket_Wrapper clientSocket;
	if((errNum = clientSocket.CreateSocket()))
	{
		SocketErrorHandler("������ �������� ������", errNum, clientSocket);
		return -1;
	}
	if((errNum = clientSocket.ConnectSocket(serverPort, serverAddress)))
	{
		SocketErrorHandler("������ ���������� � ��������", errNum, clientSocket);
		return -1;
	}

	do
	{
		std::getline(std::cin, msgData);


		if((errNum = clientSocket.SendPacket(msgData)))
		{
			SocketErrorHandler("������ �������� ���������", errNum, clientSocket);
			return -1;
		}
		std::string rcvBuf = "";
		if((errNum = clientSocket.RecievePacket(rcvBuf)))
		{
			SocketErrorHandler("������ ��������� ���������", errNum, clientSocket);
			return -1;
		}
		std::cout << rcvBuf << endLine;
	}while(msgData != "exit");

	if((errNum = TCP_Socket_Wrapper::CloseSocketAPI()))
	{
		std::cout << "������ �������� API " << errNum << endLine;
		return -1;
	}

	return 0;
}



