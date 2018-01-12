#ifndef SERIAL_H_
#define SERIAL_H_

#ifndef _WINSOCK2API_
#include <winsock2.h>
#endif // !_WINSOCK2API_
#include <string>
#include <iostream>
#include <Winspool.h>
#include <thread>

#define MAX_WIN_PORT 255

enum BAUD_RATE
{
	BAUD_RATE_300 = 300,
	BAUD_RATE_600 = 600,
	BAUD_RATE_1200 = 1200,
	BAUD_RATE_2400 = 2400,
	BAUD_RATE_4800 = 4800,
	BAUD_RATE_9600 = 9600,
	BAUD_RATE_19200 = 19200,
	BAUD_RATE_38400 = 38400,
	BAUD_RATE_57600 = 57600,
	BAUD_RATE_115200 = 115200
};

struct Port
{
	unsigned char Id;	
	bool Availbl;
};

struct PortInfo
{
	Port p[MAX_WIN_PORT];
	unsigned char koll;
};


extern "C" class __declspec (dllexport)  SerialGate
{	
public:
	
	enum IN_LINES_NAME {CTS, DSR, RING, RLSD};
	enum OUT_LINES_NAME {DTR, RTS};	

	SerialGate();
	~SerialGate();	

	bool Open(int port, int baud);	
	int Send(char* buff, int szBuff);
	int Recv(char* buff, int szBuff);	
	void SetLine(OUT_LINES_NAME ln, bool state);
	bool GetLine(IN_LINES_NAME ln);
	void GetPortsInfo(PortInfo* pi);
	void Close();
	void Clean();

private:
	HANDLE m_hFile;
	bool state;
};

extern "C" class __declspec (dllexport) TCPGateClient
{
public:

	TCPGateClient();
	TCPGateClient(int32_t resp_timeout_);
	~TCPGateClient();

	bool Open(int port, const char* addr);
	bool IsOpen();

	int Send(char* buff, int szBuff);
	int Recv(char* buff, int szBuff);

	void Close();
	
private:
	bool state;
	SOCKET con_sock;
	int32_t resp_timeout;

};

extern "C" class __declspec (dllexport) TCPGateServer
{
public:

	TCPGateServer();
	~TCPGateServer();

	bool Open(int port_1_, int port_2_);
	bool IsOpen();

	bool StartServer();
	bool IsWork();

	int SendTo(int port, char* buff, int szBuff);
	int RecvFrom(int port, char* buff, int szBuff);

	void Close();

private:
	bool opened;
	bool state;

	int port_1;
	int port_2;

	SOCKET port_1_sock;
	SOCKET port_2_sock;

	SOCKET client_1_sock;
	SOCKET client_2_sock;

	void AcceptClient(SOCKET& listen_sock, SOCKET& NewClient, sockaddr_in& client_addr);

};

#endif // SERIAL_H_


