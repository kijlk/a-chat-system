#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

int main() {
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

	if (bind(Coordinator_socket, (SOCKADDR*)&Coordinator_addr, sizeof(Coordinator_addr)) == SOCKET_ERROR) {
		wprintf(L"bind failed with error %u\n", WSAGetLastError());
		closesocket(Coordinator_socket);
		WSACleanup();
		return 1;
	}
	else
		wprintf(L"bind returned success\n");

	int Timeout = 600000;
	if (setsockopt(Coordinator_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&Timeout, sizeof(int)) == SOCKET_ERROR) {
		cout << "setsockopt error:" << GetLastError() << endl;
		return 0;
	}

	string message;
	int ret = 0;
	map<int, vector<int> > servers;
	int num = 0;
	servers.clear();
	int server_port = 28010;
	while (1) {
		sockaddr_in Client_addr;
		memset(&Client_addr, 0, sizeof(Client_addr));
		char buf[100] = { '\0' };
		int addr_len = sizeof(SOCKADDR);
		ret = recvfrom(Coordinator_socket, buf, 100, 0, (SOCKADDR*)&Client_addr, &addr_len);
		message = buf;
		if (message.size()) {
			cout << "From : " << Client_addr.sin_port << ": " << message << endl;
		}
		if (message == "create_server") {
			system("start D:\\vs_code\\test_code\\others\\computer_network\\server\\Debug\\server.exe");
			cout << "A server has been created.Port: " << server_port << endl;
			message = "A server has been created. Port: " + to_string(server_port);
			servers[server_port].push_back(Client_addr.sin_port);
			server_port++;
			ret= sendto(Coordinator_socket, message.c_str(), message.size(), 0, (SOCKADDR*)&Client_addr, addr_len);
		}
		else if (message == "exit()") {
			int flag = 0;
			for (map<int, vector<int> >::iterator ite = servers.begin(); ite != servers.end(); ite++) {
				for (vector<int>::iterator i = ite->second.begin(); i != ite->second.end();i++) {
					if (*i == Client_addr.sin_port) {
						ite->second.erase(i);
						flag = 1;
						message = "has been removed from dialogue list";
						ret = sendto(Coordinator_socket, message.c_str(), message.size(), 0, (SOCKADDR*)&Client_addr, addr_len);
						break;
					}
					if (flag) break;
				}
				if (flag) break;
			}
		}else if (message.substr(0, 4) == "join") {
			servers[atoi(message.substr(9).c_str())].push_back(Client_addr.sin_port);
			message = message.substr(9) + " has been added.";
			ret = sendto(Coordinator_socket, message.c_str(), message.size(), 0, (SOCKADDR*)&Client_addr, addr_len);
		}
		else {
			message = "succeed : connecting to coordinator";
			ret = sendto(Coordinator_socket, message.c_str(), message.size(), 0, (SOCKADDR*)&Client_addr, addr_len);
		}
		if (ret==0||ret == SOCKET_ERROR) break;
	}
	closesocket(Coordinator_socket);
	WSACleanup();
}