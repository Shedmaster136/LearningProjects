//заметки по программированию сокетов
//Sockaddr и sockaddr_in структуры имеют одинаковый размер
//sockaddr_in - приспособлена под ipv4.
//там, где у sockaddr char[14] в sockaddr_in
//4 байта отводится под хранение адреса, 2 под порт (u_short)
//Остальные 8 заполнены нулями.
//sockaddr_storage - исходная структура и у нее нет границ (по стандарту rfc)
//поэтому и необходимо в accept передавать длинну. accept принимает любые стеки соединений.
//если же мы заранее определились, что не хотимдругих стеков, то при получении ошибки переполнения (такая есть)
//от accept, просто её игнорируем.

//и сервер и клиент оперируют минимум 2 сокетами:
//1ый сокет - содержит информацию об исходящем интерфейсе,
//2ой сокет - содержит информацию об интерфейсе, с которым
// устанавливается соединение. 2ой сокет получает сервер функцией accept, клиент получает при помощи
//функции connect.
//так что функции send и recv не отличаются для сервера и клиента.

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>


//переделать функцию получения ошибок на enum и getlast error
//добавить обработку ошибок в функции accept и connect которые позволили бы
//пользоваться неблокируемыми сокетами


//Максимальная длина информации о размере собщения
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
	//получение строки неопределенного размера.
	int RecievePacket(std::string &);
	int CloseSocket();
	std::string GetIPAddr();
private:
	SOCKET socHandle;
	sockaddr_in socParam;
	//буффер для чтения сообщений произвольной длины при неблокирующем режиме
	std::string workBuff;
	size_t msgSize;
};
//функция написана таким образом, чтобы поддерживать работу в блокирующем и неблокирующем режимах


int TCP_Socket_Wrapper::SendPacket(std::string sData)
{
	std::string sPacket = std::to_string(sData.size());
	//добавление длины в начале сообщения
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
}//для неблокирующего нужно хранить состояния: workBuf, msgSize

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

//только для клиента
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
	//если не получится таким образом, то определить конструктор
	return 0;
}


std::string TCP_Socket_Wrapper::GetIPAddr()
{
	std::string ipV4address(inet_ntoa(socParam.sin_addr));
	return ipV4address;
}

//только для сервера
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
	//если не получится таким образом, то определить конструктор
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
	//привязка к стандартному пути (0.0.0.0)
	if(ipAddr.empty())
	{
		socParam.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	}
	else
	{
		socParam.sin_addr.S_un.S_addr = inet_addr(ipAddr.c_str());
	}
	//Привязка к статически выделенному порту (если 0, то порт будет динамический)
	socParam.sin_port = htons(portNum);
}

//только для сервера
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
//только для сервера
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

//Закрывает WinSock
int TCP_Socket_Wrapper::CloseSocketAPI()
{
	if(WSACleanup() == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}
	return 0;
}

//Возвращает имя локального хоста
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
