#define _WIN32_WINNT _WIN32_WINNT_WIN10
#define PORT 27015

#include <iostream>
#include <bits/stdc++.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include "helper.cpp"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

bool initialize()
{
    WSADATA data;
    int init_check = WSAStartup(MAKEWORD(2, 2), &data);

    return (init_check == 0);
}

bool setup_address(sockaddr_in &curr_addr, int port)
{
    curr_addr.sin_family = AF_INET;
    curr_addr.sin_port = htons(port);
    int check = inet_pton(AF_INET, "127.0.0.1", &curr_addr.sin_addr);
    return (check == 1);
}

int main()
{

    cout << "Firing the server..." << endl;

    if (!initialize())
    {
        cout << "Could not initialize winsock, startup failed" << endl;
        return 1;
    }

    SOCKET server_listen = socket(AF_INET, SOCK_STREAM, 0);

    if (server_listen == INVALID_SOCKET)
    {
        cout << "Could not create socket, terminating server process" << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in server_address;

    bool check_setup = setup_address(server_address, PORT);
    if (!check_setup)
    {
        cout << "Could not assign address to structure, terminating server process" << endl;
        closesocket(server_listen);
        WSACleanup();
        return 1;
    }

    if (bind(server_listen, reinterpret_cast<sockaddr *>(&server_address), sizeof(server_address)) == SOCKET_ERROR)
    {
        cout << "Could not bind to port" << endl;
        closesocket(server_listen);
        WSACleanup();
        return 1;
    }

    if (listen(server_listen, SOMAXCONN) == SOCKET_ERROR)
    {
        cout << "Could not listen on the socket" << endl;
        closesocket(server_listen);
        WSACleanup();
        return 1;
    }

    cout << "Server Listnening on port: " << PORT << endl;

    SOCKET client_socket = accept(server_listen, nullptr, nullptr);

    if (client_socket == SOCKET_ERROR)
    {
        cout << "Could not get client socket" << endl;
        closesocket(server_listen);
        WSACleanup();
        return 0;
    }

    char buffer[2048];
    int bytes_recvd = recv(client_socket, buffer, sizeof(buffer), 0);

    if (bytes_recvd == 0)
    {
        cout << "Received no data form client, terminating server" << endl;
        closesocket(server_listen);
        WSACleanup();
        return 1;
    }

    string message(buffer, bytes_recvd);
    cout << "Received Message: " << message << " (" << bytes_recvd << " Bytes)" << endl;

    char send_message[] = "Initial Message Received";

    int bytes_sent = send(client_socket, send_message, sizeof(send_message), 0);
    cout << "Sent Bytes: " << bytes_sent << endl;

    WSACleanup();
    return 0;
}