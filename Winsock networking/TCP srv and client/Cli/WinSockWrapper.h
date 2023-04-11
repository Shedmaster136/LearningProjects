//заметки по программированию сокетов
//Sockaddr и sockaddr_in структуры имеют одинаковый размер
//sockaddr_in - приспособлена под ipv4.
//там, где у sockaddr char[14] в sockaddr_in
//4 байта отводитс€ под хранение адреса, 2 под порт (u_short)
//ќстальные 8 заполнены нул€ми.
//sockaddr_storage - исходна€ структура и у нее нет границ (по стандарту rfc)
//поэтому и необходимо в accept передавать длинну. accept принимает любые стеки соединений.
//если же мы заранее определились, что не хотимдругих стеков, то при получении ошибки переполнени€ (така€ есть)
//от accept, просто еЄ игнорируем.

//и сервер и клиент оперируют минимум 2 сокетами:
//1ый сокет - содержит информацию об исход€щем интерфейсе,
//2ой сокет - содержит информацию об интерфейсе, с которым
// устанавливаетс€ соединение. 2ой сокет получает сервер функцией accept, клиент получает при помощи
//функции connect.
//так что функции send и recv не отличаютс€ дл€ сервера и клиента.

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>


//переделать функцию получени€ ошибок на enum и getlast error
//добавить обработку ошибок в функции accept и connect которые позволили бы
//пользоватьс€ неблокируемыми сокетами.
class TCP_Socket_Wrapper
{
public:
	static int InitSocketAPI();
	int GetLastError();
	static int CloseSocketAPI();
	static int GetLocalHostName(std::string&);
	TCP_Socket_Wrapper(int, std::string);
	~TCP_Socket_Wrapper(){};
	int CreateSocket(void);
	int BindSocket(void);
	int ListenSocket(int);
	int AcceptConnection(TCP_Socket_Wrapper&);
	int ConnectSocket(int, std::string);
	int SetNonblocked();
	int Send(std::string, int);
	int Recieve(std::string &, int, int);
	int CloseSocket();
	std::string GetIPAddr();
private:
	SOCKET socHandle;
	sockaddr_in socParam;
	static const int errRetVal = -1;
};


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
	if(send(socHandle, sData.data(), sData.size(), sFlags) == SOCKET_ERROR)
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
	rData.append(recvBuf);
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

//только дл€ клиента
int TCP_Socket_Wrapper::ConnectSocket(int portNum, std::string serverAddr)
{
	//параметры сервера
	sockaddr_in newSocParam;
	ZeroMemory(&newSocParam, sizeof(newSocParam));
	newSocParam.sin_family = AF_INET;
	newSocParam.sin_addr.S_un.S_addr = inet_addr(serverAddr.c_str());
	newSocParam.sin_port = htons(portNum);
	//соединение
	if(connect(socHandle, (sockaddr*)&newSocParam, sizeof(newSocParam)) == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}
	//как то создать новый объект сокета и вернуть.
	//если не получитс€ таким образом, то определить конструктор
	return 0;
}


std::string TCP_Socket_Wrapper::GetIPAddr()
{
	std::string ipV4address(inet_ntoa(socParam.sin_addr));
	return ipV4address;
}

//только дл€ сервера
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
	//как то создать новый объект сокета и вернуть.
	//если не получитс€ таким образом, то определить конструктор
	clientSoc.socHandle = newConnectionSock;
	clientSoc.socParam.sin_addr.S_un.S_addr = newSocParam.sin_addr.S_un.S_addr;
	clientSoc.socParam.sin_family = newSocParam.sin_family;
	clientSoc.socParam.sin_port = newSocParam.sin_port;
	return 0;
}


TCP_Socket_Wrapper::TCP_Socket_Wrapper(int portNum = 0, std::string ipAddr = "")
{
	socHandle = INVALID_SOCKET;
	ZeroMemory(&socParam, sizeof(socParam));
	socParam.sin_family = AF_INET;
	//прив€зка к стандартному пути (0.0.0.0)
	if(ipAddr.empty())
	{
		socParam.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	}
	else
	{
		socParam.sin_addr.S_un.S_addr = inet_addr(ipAddr.c_str());
	}
	//ѕрив€зка к статически выделенному порту (если 0, то порт будет динамический)
	socParam.sin_port = htons(portNum);
}

//только дл€ сервера
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
//только дл€ сервера
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


//инициализирует WinSock
int TCP_Socket_Wrapper::InitSocketAPI()
{
	WSADATA wsaData;
	return WSAStartup(MAKEWORD(2,2), &wsaData);
}

//«акрывает WinSock
int TCP_Socket_Wrapper::CloseSocketAPI()
{
	if(WSACleanup() == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}
	return 0;
}

//¬озвращает им€ локального хоста
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
