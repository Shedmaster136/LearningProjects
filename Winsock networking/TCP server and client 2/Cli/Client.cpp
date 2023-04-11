/*
 * Client.cpp
 *
 *  Created on: 13 ���. 2020 �.
 *      Author: Steep
 */

#include "WinSockWrapper.h"
#include <iostream>
#include <fstream>

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
	std::ifstream sendFile;
	std::string fileLine;
	std::string rcvBuf;
	int errNum = 0;
	std::string cmdData = "";
	if(argc != 3)
	{
		std::cout << "�������� ���������� ���������� | CMD Argument Error" << endLine;
		return -1;
	}
	int serverPort = 0;
	serverPort = std::stoi(std::string(argv[1]));
	if(serverPort == 0)
	{
		std::cout << "�������� ���� | No server on this port" << endLine;
		return -1;
	}
	std::string serverAddress(argv[2]);

	if((errNum = TCP_Socket_Wrapper::InitSocketAPI()))
	{
		std::cout << "������ ������������� | Winsock Intitialization Failed"<< errNum << endLine;
		return -1;
	}

	TCP_Socket_Wrapper clientSocket;
	if((errNum = clientSocket.CreateSocket()))
	{
		SocketErrorHandler("������ �������� ������ | Socket Creation Error", errNum, clientSocket);
		return -1;
	}
	if((errNum = clientSocket.ConnectSocket(serverPort, serverAddress)))
	{
		SocketErrorHandler("������ ���������� � �������� | Server Connection Error", errNum, clientSocket);
		return -1;
	}

	while(cmdData != "exit")
	{
		std::cout << "�������� ���� ��� �������� �� ������. | Choose File to Send"<< endLine;
		std::getline(std::cin, cmdData);
		sendFile.open(cmdData);
		if(sendFile.is_open())
		{
			while (std::getline(sendFile, fileLine))
			{
				if((errNum = clientSocket.SendPacket(fileLine)))
				{
					SocketErrorHandler("������ �������� ��������� | Message Send Error", errNum, clientSocket);
					return -1;
				}
				if((errNum = clientSocket.RecievePacket(rcvBuf)))
				{
					SocketErrorHandler("������ ��������� ��������� | Message Recieve Error", errNum, clientSocket);
					return -1;
				}
			}
			std::cout << rcvBuf << endLine;
		}
		else
		{
			std::cout << "������������ ���� | Wrong Filename"<<endLine;
		}

	}

	if((errNum = TCP_Socket_Wrapper::CloseSocketAPI()))
	{
		std::cout << "������ �������� API | API Close Error" << errNum << endLine;
		return -1;
	}

	return 0;
}



