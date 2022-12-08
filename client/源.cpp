#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <process.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

int main(void)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		printf("WSAStartup failed with error: %d\n", err);
		return 1;
	}

	SOCKET Coordinator_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (Coordinator_socket == INVALID_SOCKET) {
		int a = WSAGetLastError();
		printf("socket function failed with error = %d\n", a);
		WSACleanup();
		return 1;
	}
	// The socket address to be passed to bind
	sockaddr_in Coordinator_addr;
	memset(&Coordinator_addr, 0, sizeof(Coordinator_addr));
	Coordinator_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	Coordinator_addr.sin_port = htons(27015);
	Coordinator_addr.sin_family = AF_INET;

	//Coordinator
	string message = "connect to coordinator";
	int ret = sendto(Coordinator_socket, message.c_str(), message.size(), 0, (SOCKADDR*)&Coordinator_addr, sizeof(sockaddr_in));
	if (ret == SOCKET_ERROR) {
		printf("sendto : SOCKET_ERROR\n");
	}
	char buf[100] = { '\0' };
	recvfrom(Coordinator_socket, buf, 100, 0, NULL, NULL);
	cout << "From coordinator: " << buf << endl;

	cout << "Choose : 'create_server' or 'join_the_xxx' " << endl;
	cin >> message;
	int server_port = -1;
	if (message == "create_server") {
		int ret = sendto(Coordinator_socket, message.c_str(), message.size(), 0, (SOCKADDR*)&Coordinator_addr, sizeof(sockaddr_in));
		if (ret == SOCKET_ERROR) {
			printf("sendto : SOCKET_ERROR\n");
		}
		char buf[100] = { '\0' };
		recvfrom(Coordinator_socket, buf, 100, 0, NULL, NULL);
		cout << "From coordinator: " << buf << endl;
		message = buf;
		server_port = atoi((message.substr(32)).c_str());
	}
	else {
		int ret = sendto(Coordinator_socket, message.c_str(), message.size(), 0, (SOCKADDR*)&Coordinator_addr, sizeof(sockaddr_in));
		if (ret == SOCKET_ERROR) {
			printf("sendto : SOCKET_ERROR\n");
		}
		char buf[100] = { '\0' };
		recvfrom(Coordinator_socket, buf, 100, 0, NULL, NULL);
		cout << "From coordinator: " << buf << endl;
		server_port = atoi(message.substr(9).c_str());
		Sleep(100);
	}


	//Server
	SOCKET Server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Server_socket == INVALID_SOCKET) {
		int a = WSAGetLastError();
		printf("socket function failed with error = %d\n", a);
		WSACleanup();
		return 1;
	}
	// The socket address to be passed to bind
	sockaddr_in Server_addr;
	memset(&Server_addr, 0, sizeof(Server_addr));
	Server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	Server_addr.sin_port = htons(server_port);
	Server_addr.sin_family = AF_INET;
	if (connect(Server_socket, (sockaddr*)&Server_addr, sizeof(sockaddr_in)) == SOCKET_ERROR){
		wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
		if (closesocket(Server_socket) == SOCKET_ERROR)
			wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	int cnt = 0;
	while (1) {
		cnt++;
		memset(buf, '\0', sizeof(buf));
		cin.getline(buf, 100);
		send(Server_socket, buf, 100, NULL);
		message = buf;
		if (buf == "exit()") {
			sendto(Coordinator_socket, message.c_str(), message.size(), 0, (SOCKADDR*)&Coordinator_addr, sizeof(sockaddr_in));
			char buf[100] = { '\0' };
			recvfrom(Coordinator_socket, buf, 100, 0, NULL, NULL);
			string receive_message = buf;
			cout << "From coordinator: " << receive_message << endl;
			closesocket(Coordinator_socket);
			break;
		}
		if (cnt > 1) {
			memset(buf, '\0', sizeof(buf));
			recv(Server_socket, buf, 100, NULL);
			message = buf;
			if (message.size() != 0) {
				cout << message << endl;
			}
		}
	}
	closesocket(Coordinator_socket);
	closesocket(Server_socket);
	WSACleanup();
	return 0;
}
