#define _WIN32_WINNT _WIN32_WINNT_WIN10
#define DESTPORT 27015

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

void fill_buffer(string s, char *curr_buff)
{
    int n = s.length();

    for (int i = 0; i < n; i++)
    {
        curr_buff[i] = s[i];
    }
    return;
}

int main()
{

    cout << "Initializing Client..." << endl;

    if (!initialize())
    {
        cout << "Could not initialize winsock, startup failed" << endl;
        return 1;
    }

    SOCKET client = socket(AF_INET, SOCK_STREAM, 0);

    if (client == INVALID_SOCKET)
    {
        cout << "Could not create socket, terminating process" << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in server_address;

    bool check_server_addr = setup_address(server_address, DESTPORT);
    if (!check_server_addr)
    {
        cout << "Could not cast server address to structure, terminating process" << endl;
        closesocket(client);
        WSACleanup();
        return 1;
    }

    if (connect(client, reinterpret_cast<sockaddr *>(&server_address), sizeof(server_address)) == SOCKET_ERROR)
    {
        cout << "Could not connect to server" << endl;
        closesocket(client);
        WSACleanup();
        return 1;
    }

    cout << "Connected to host at: " << "127.0.0.1:" << DESTPORT << endl;

    string handshake_text = "This is client at IP localhost port: " + to_string(DESTPORT);

    char buffer[handshake_text.length()];

    fill_buffer(handshake_text, buffer);

    int bytes_sent = send(client, buffer, sizeof(buffer), 0);

    if (bytes_sent == 0)
    {
        cout << "Failed to send handshake, terminating......" << endl;
        closesocket(client);
        WSACleanup();
        return 1;
    }

    WSACleanup();
    return 0;
}