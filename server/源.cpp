#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h> 
#include <iostream>
#include <process.h>
using namespace std;
#pragma comment(lib,"ws2_32.lib")

void NewClients(LPVOID para) {
    SOCKET Client = *((SOCKET*)para);
    char buf[100] = { '\0' };
    sockaddr_in client_addr;
    int len = sizeof(sockaddr);
    getpeername(Client, (sockaddr*)&client_addr, &len);
    cout << client_addr.sin_addr.S_un.S_addr << ":" << client_addr.sin_port << " has connected!\n";
    string message = {0};
    while (message != "exit()") {
        memset(buf, '\0', sizeof(buf));
        recv(Client, buf, 100, NULL);
        message = buf;
        if (message.length()) {
            cout << "Receive from" << client_addr.sin_port << ": " << message << endl;
            string se = "Server received the message : ";
            se += message;
            send(Client, se.c_str(), 100, NULL);
        }
    }
}

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
    int sinport = 28010;
    Server_addr.sin_port = htons(sinport);
    Server_addr.sin_family = AF_INET;
    int len = sizeof(sockaddr_in);
    while (bind(Server_socket, (sockaddr*)&Server_addr, len) == SOCKET_ERROR) {
        sinport++;
        Server_addr.sin_port = htons(sinport);
    }
    cout << "Server Port: " << sinport << endl;
    if (listen(Server_socket, 5) == SOCKET_ERROR) {
        cout << "Error in listen :" << WSAGetLastError() << endl;
        return 0;
    }


    while (1) {
        SOCKET temp = accept(Server_socket, NULL, NULL);
        if (temp>=0) _beginthread(NewClients, NULL, &temp);
    }

    closesocket(Server_socket);
    WSACleanup();
    return 0;
}