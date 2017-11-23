#define _CRT_SECURE_NO_WARNINGS

#include "SockUtils.h"
#include <winsock2.h>
//#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;

WORD wVersionRequested;
WSADATA wsaData;
int err;

unsigned int DefaultPort = 10001;
const char *DefaultServerAddress = "127.0.0.1";
bool IsQuitRequested;

class ClientConnectionInfo
{
public:
	int ClientId;
	SOCKET clientSocket;
};

bool InitSocketsLib()
{
	wVersionRequested = MAKEWORD( 2, 2 );
	err=WSAStartup( wVersionRequested, &wsaData );
	return (err==0);
}

bool ReleaseSocketsLib()
{
	err=WSACleanup();
	return (err==0);
}

unsigned long __stdcall ReceiveThreadFunction(void* pParam)
{
	ClientConnectionInfo *cli = (ClientConnectionInfo *)pParam;
	SOCKET clientSocket=cli->clientSocket;
    int bufSize = 100;
	
	char *buf = new char[bufSize];
	memset(buf, 0, bufSize);
    
	while(recv(clientSocket, buf, bufSize-1, 0))
    {
		if(strncmp(buf, "quit", 4)==0)
		{
			cout<<"Client #"<<cli->ClientId<<": quit command received!"<<endl;
			IsQuitRequested = true;
			break;
		}
		else
		{
			cout<<"Client #"<<cli->ClientId<<": "<<buf<<endl;
			memset(buf, 0, bufSize);
		}
	}

	closesocket(clientSocket);

	cout<<"Client #"<<cli->ClientId<<" disconnected."<<endl;

	delete cli;

	return 0;
}

void TestSockets()
{
	int bufSize = 128;

    SOCKET s=socket(AF_INET,SOCK_STREAM,0);
    sockaddr_in dest_addr;
    dest_addr.sin_family=AF_INET;
    dest_addr.sin_port=htons(80);
    dest_addr.sin_addr.s_addr=inet_addr("91.202.128.77");
    connect(s,(sockaddr *)&dest_addr,sizeof(dest_addr));
    
    char *buf = new char[bufSize]; 
	memset(buf, 0, bufSize);

	int res = 0;

	strcpy(buf, "GET / \n\n");
    res = send(s, buf, strlen(buf), 0);

	memset(buf, 0, bufSize);
	res = recv(s, buf, bufSize-1, 0);

	cout<<"Received string:\n";
    while(res!=0)
    {
        cout<<buf;
		memset(buf, 0, bufSize);
		res = recv(s, buf, bufSize-1, 0);
    }
	
	cout<<endl;

    closesocket(s);
}

void TestServerSocket()
{
	int clientsCnt = 0;

	//tutorial
    SOCKET s=socket(AF_INET,SOCK_STREAM,0);
    
	sockaddr_in local_addr;
    local_addr.sin_family=AF_INET;
    local_addr.sin_port=htons(DefaultPort);
    local_addr.sin_addr.s_addr=0;
    
	bind(s,(sockaddr *) &local_addr,sizeof(local_addr));
	
    int c=listen(s,5);
    
	IsQuitRequested = false;
	cout<<"Server receive ready"<<endl;
    
	SOCKET client_socket;    
    sockaddr_in client_addr;
    int client_addr_size=sizeof(client_addr);
    
//	fd_set socksToCheck;
//	TIMEVAL timeOut;
//	timeOut.tv_sec = 1;

	while(!IsQuitRequested)
	{
//		FD_ZERO(&socksToCheck);
		//FD_SET(s, &socksToCheck);
		//int res = select(1, &socksToCheck,  0, 0, &timeOut);
		//if(res==1)
		//{
			client_socket=accept(s,(sockaddr *)&client_addr, &client_addr_size);

			send(client_socket, "AA", 2, 0);

			clientsCnt++;   
			cout<<"Client #"<<clientsCnt<<" connected."<<endl;
		
			ClientConnectionInfo *cli = new  ClientConnectionInfo();
			cli->ClientId = clientsCnt;
			cli->clientSocket = client_socket;

			DWORD threadId;
			CreateThread(NULL, NULL, ReceiveThreadFunction, cli, NULL,&threadId);
		//}
    }
	cout<<"Server shutting down."<<endl;
}

void TestSendToServer()
{
    SOCKET s=socket(AF_INET,SOCK_STREAM,0);
    sockaddr_in dest_addr;
    dest_addr.sin_family=AF_INET;
	dest_addr.sin_port=htons(DefaultPort);
    dest_addr.sin_addr.s_addr=inet_addr(DefaultServerAddress);
    
	

	if(connect(s,(sockaddr *)&dest_addr,sizeof(dest_addr))!=0)
	{
		cout<<"Error connecting to server!";
		return;
	}
    
	string dataToSend;

	cin>>dataToSend;

	while(send(s, dataToSend.c_str(), dataToSend.length(), 0)!=SOCKET_ERROR && (dataToSend!="exit"))
    {
		 getline(cin, dataToSend);
    }
	
    closesocket(s);
}