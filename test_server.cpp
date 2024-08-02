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

void generate_unit_load()
{
    SOCKET client = socket(AF_INET, SOCK_STREAM, 0);

    if (client == INVALID_SOCKET)
    {
        cout << "Could not create socket, terminating process" << endl;
        WSACleanup();
        return;
    }

    sockaddr_in server_address;

    bool check_server_addr = setup_address(server_address, DESTPORT);
    if (!check_server_addr)
    {
        cout << "Could not cast server address to structure, terminating process" << endl;
        closesocket(client);
        WSACleanup();
        return;
    }

    if (connect(client, reinterpret_cast<sockaddr *>(&server_address), sizeof(server_address)) == SOCKET_ERROR)
    {
        cout << "Could not connect to server" << endl;
        closesocket(client);
        WSACleanup();
        return;
    }

    cout << "Connected to host at: " << "127.0.0.1:" << DESTPORT << endl;

    string handshake_text = "f22x";

    char buffer[handshake_text.length()];

    fill_buffer(handshake_text, buffer);

    int bytes_sent = send(client, buffer, sizeof(buffer), 0);

    if (bytes_sent == 0)
    {
        cout << "Failed to send handshake, terminating......" << endl;
        closesocket(client);
        WSACleanup();
        return;
    }

    char recv_buffer[31];

    int bytes_recv = recv(client, recv_buffer, sizeof(recv_buffer), 0);

    if (bytes_recv > 0)
    {
        cout << "Message Received from Server (" << bytes_recv << " Bytes) : " << recv_buffer << endl;
    }
}

int main()
{

    cout << "Initializing Client..." << endl;

    if (!initialize())
    {
        cout << "Could not initialize winsock, startup failed" << endl;
        return 1;
    }

    cout << "Enter the number of calls you wish to make: ";
    int n;
    cin >> n;

    for (int i = 0; i < n; i++)
        generate_unit_load();

    //WSACleanup();
    return 0;
}