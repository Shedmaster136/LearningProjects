//������� �� ���������������� �������
//Sockaddr � sockaddr_in ��������� ����� ���������� ������
//sockaddr_in - ������������� ��� ipv4.
//���, ��� � sockaddr char[14] � sockaddr_in
//4 ����� ��������� ��� �������� ������, 2 ��� ���� (u_short)
//��������� 8 ��������� ������.
//sockaddr_storage - �������� ��������� � � ��� ��� ������ (�� ��������� rfc)
//������� � ���������� � accept ���������� ������. accept ��������� ����� ����� ����������.
//���� �� �� ������� ������������, ��� �� ����������� ������, �� ��� ��������� ������ ������������ (����� ����)
//�� accept, ������ � ����������.

//� ������ � ������ ��������� ������� 2 ��������:
//1�� ����� - �������� ���������� �� ��������� ����������,
//2�� ����� - �������� ���������� �� ����������, � �������
// ��������������� ����������. 2�� ����� �������� ������ �������� accept, ������ �������� ��� ������
//������� connect.
//��� ��� ������� send � recv �� ���������� ��� ������� � �������.

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>


//���������� ������� ��������� ������ �� enum � getlast error
//�������� ��������� ������ � ������� accept � connect ������� ��������� ��
//������������ �������������� ��������


//������������ ����� ���������� � ������� ��������
const int MAX_MSG_SIZE_LEN = 4;

class TCP_Socket_Wrapper
{
public:
	static int InitSocketAPI();
	int GetLastError();
	static int CloseSocketAPI();
	static int GetLocalHostName(std::string&);
	TCP_Socket_Wrapper(int, std::string);
	int CreateSocket(void);
	int BindSocket(void);
	int ListenSocket(int);
	int AcceptConnection(TCP_Socket_Wrapper&);
	int ConnectSocket(int, std::string);
	int SetNonblocked();
	int Send(std::string, int);
	int SendPacket(std::string);
	int Recieve(std::string &, int, int);
	//��������� ������ ��������������� �������.
	int RecievePacket(std::string &);
	int CloseSocket();
	std::string GetIPAddr();
private:
	SOCKET socHandle;
	sockaddr_in socParam;
	//������ ��� ������ ��������� ������������ ����� ��� ������������� ������
	std::string workBuff;
	size_t msgSize;
};
//������� �������� ����� �������, ����� ������������ ������ � ����������� � ������������� �������


int TCP_Socket_Wrapper::SendPacket(std::string sData)
{
	std::string sPacket = std::to_string(sData.size());
	//���������� ����� � ������ ���������
	if(sPacket.length() > MAX_MSG_SIZE_LEN)
	{
		return -1;
	}
	else
	{
		while(sPacket.length() < MAX_MSG_SIZE_LEN)
		{
			sPacket.insert(0, "0");
		}
	}
	sPacket.append(sData);
	return this->Send(sPacket, 0);
}

int TCP_Socket_Wrapper::RecievePacket(std::string & rData)
{
	int lastError = 0;
	if(msgSize == 0)
	{
		lastError = this->Recieve(workBuff, MAX_MSG_SIZE_LEN,0);
		if(lastError != 0)
		{
			return lastError;
		}
		msgSize = std::stoi(workBuff);
		workBuff.clear();
	}
	while (workBuff.length() < msgSize)
	{
		lastError = this->Recieve(rData, msgSize - workBuff.length(),0);
		if(lastError != 0)
		{
			if(lastError != WSAEWOULDBLOCK)
			{
				workBuff.clear();
				rData.clear();
				msgSize = 0;
			}
			return lastError;
		}
		workBuff.append(rData);
		rData.clear();
	}
	rData.append(workBuff);
	msgSize = 0;
	return 0;
}//��� �������������� ����� ������� ���������: workBuf, msgSize

int TCP_Socket_Wrapper::CloseSocket()
{
	if(closesocket(socHandle) == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}
	return 0;
}



int TCP_Socket_Wrapper::Send(std::string sData, int sFlags = 0)
{
	if(send(socHandle, sData.c_str(), sData.size(), sFlags) == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}
	return 0;
}

int TCP_Socket_Wrapper::Recieve(std::string& rData, int recvLen, int rFlags = 0)
{
	char recvBuf[recvLen];
	if(recv(socHandle, recvBuf, recvLen, rFlags) == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}
	rData.clear();
	rData.append(recvBuf, 0, recvLen);
	return 0;
}
int TCP_Socket_Wrapper::SetNonblocked()
{
    u_long socMode = 0;
    if(ioctlsocket(socHandle, FIONBIO, &socMode) == SOCKET_ERROR)
    {
        return WSAGetLastError();
    }
    return 0;
}

//������ ��� �������
int TCP_Socket_Wrapper::ConnectSocket(int portNum, std::string serverAddr)
{
	//��������� �������
	sockaddr_in newSocParam;
	ZeroMemory(&newSocParam, sizeof(newSocParam));
	newSocParam.sin_family = AF_INET;
	newSocParam.sin_addr.S_un.S_addr = inet_addr(serverAddr.c_str());
	newSocParam.sin_port = htons(portNum);
	//����������
	if(connect(socHandle, (sockaddr*)&newSocParam, sizeof(newSocParam)) == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}
	//��� �� ������� ����� ������ ������ � �������.
	//���� �� ��������� ����� �������, �� ���������� �����������
	return 0;
}


std::string TCP_Socket_Wrapper::GetIPAddr()
{
	std::string ipV4address(inet_ntoa(socParam.sin_addr));
	return ipV4address;
}

//������ ��� �������
int TCP_Socket_Wrapper::AcceptConnection(TCP_Socket_Wrapper & clientSoc)
{
	sockaddr_in newSocParam;
	int newParamLen = sizeof(newSocParam);
	ZeroMemory(&newSocParam, sizeof(newSocParam));
	SOCKET newConnectionSock = INVALID_SOCKET;
	newConnectionSock = accept(socHandle, (sockaddr*)&newSocParam, &newParamLen);
	if(newConnectionSock == INVALID_SOCKET)
	{
		return WSAGetLastError();
	}
	//��� �� ������� ����� ������ ������ � �������.
	//���� �� ��������� ����� �������, �� ���������� �����������
	clientSoc.socHandle = newConnectionSock;
	clientSoc.socParam.sin_addr.S_un.S_addr = newSocParam.sin_addr.S_un.S_addr;
	clientSoc.socParam.sin_family = newSocParam.sin_family;
	clientSoc.socParam.sin_port = newSocParam.sin_port;
	return 0;
}


TCP_Socket_Wrapper::TCP_Socket_Wrapper(int portNum = 0, std::string ipAddr = "")
{
	workBuff = "";
	msgSize = 0;
	socHandle = INVALID_SOCKET;
	ZeroMemory(&socParam, sizeof(socParam));
	socParam.sin_family = AF_INET;
	//�������� � ������������ ���� (0.0.0.0)
	if(ipAddr.empty())
	{
		socParam.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	}
	else
	{
		socParam.sin_addr.S_un.S_addr = inet_addr(ipAddr.c_str());
	}
	//�������� � ���������� ����������� ����� (���� 0, �� ���� ����� ������������)
	socParam.sin_port = htons(portNum);
}

//������ ��� �������
int TCP_Socket_Wrapper::ListenSocket(int maxConnect)
{
	if(maxConnect == 0 || maxConnect > SOMAXCONN)
	{
		maxConnect = SOMAXCONN;
	}
	if(listen(socHandle, maxConnect) == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}
	return 0;
}
//������ ��� �������
int TCP_Socket_Wrapper::BindSocket()
{

	if(bind(socHandle, (sockaddr*) &socParam, sizeof(socParam)) == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}
	return 0;
}

int TCP_Socket_Wrapper::CreateSocket()
{
	socHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(socHandle == INVALID_SOCKET)
	{
		return WSAGetLastError();
	}
	return 0;
}


//�������������� WinSock
int TCP_Socket_Wrapper::InitSocketAPI()
{
	WSADATA wsaData;
	return WSAStartup(MAKEWORD(2,2), &wsaData);
}

//��������� WinSock
int TCP_Socket_Wrapper::CloseSocketAPI()
{
	if(WSACleanup() == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}
	return 0;
}

//���������� ��� ���������� �����
//std::string & hostName
int TCP_Socket_Wrapper::GetLocalHostName(std::string &hostName)
{
	char charHostName[256];
	if(gethostname(charHostName, sizeof(charHostName)) == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}
	hostName.assign(charHostName, sizeof(charHostName));
	hostName.resize(hostName.find_first_of('\0'));
	return 0;
}
