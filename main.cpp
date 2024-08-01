#define _WIN32_WINNT _WIN32_WINNT_WIN10
#define PORT 27015

#include <iostream>
#include <bits/stdc++.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>

#include <pthread.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

bool initialize()
{
    WSADATA data;
    int init_check = WSAStartup(MAKEWORD(2, 2), &data);

    return (init_check == 0);
}

string get_code(int a, int b)
{
    string secret_code = "XG452552" + to_string(a + b) + "00065" + to_string(abs(a - b)) + "f0xE";
    return secret_code;
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
void send_code(SOCKET &client, char a, char b)
{

    string code = get_code(a - 'a', b - 'a');
    char send_buff[code.length()];

    fill_buffer(code, send_buff);

    int bytes_sent = send(client, send_buff, sizeof(send_buff), 0);

    cout << "Sent code: " << code << " (" << bytes_sent << " Bytes)" << endl;
}

bool setup_address(sockaddr_in &curr_addr, int port)
{
    curr_addr.sin_family = AF_INET;
    curr_addr.sin_port = htons(port);
    int check = inet_pton(AF_INET, "127.0.0.1", &curr_addr.sin_addr);
    return (check == 1);
}

void send_ok_handshake(SOCKET &client)
{

    char ok_buffer[] = "Hello World Server Test Deploy";
    int bytes_sent = send(client, ok_buffer, sizeof(ok_buffer), 0);
    cout << "Sent decoy message to client: " << bytes_sent << " Bytes" << endl;
    return;
}
void handle_client(SOCKET client, int i)
{
    cout << "New connection established, active connections: " << i << endl;
    while (1)
    {
        char buffer[1024];

        int bytes_recvd = recv(client, buffer, sizeof(buffer), 0);

        if (bytes_recvd < 0)
        {
            cout << "Invalid Message, closing connection" << endl;

            int check_shutdown = shutdown(client, SD_SEND);

            if (check_shutdown == SOCKET_ERROR)
            {
                cout << "Failed to terminate connection, terminating server process..." << endl;
                // auto it = find(clients.begin(), clients.end(), client);
                // if (it != clients.end())
                //     clients.erase(it);
                // return;
            }

            return;
        }
        else if(bytes_recvd==0) continue;

        cout << "Recieved " << bytes_recvd << " Bytes" << endl;
        cout << "Message Received: " << buffer << endl;

        if (buffer[3] == 'x' && buffer[0] == 'f')
            send_code(client, buffer[1], buffer[2]);
        else if (buffer[0] == 'c' && buffer[1] == 'q' && buffer[2] == 'q' && buffer[3] == 'q')
            break;
        else
            send_ok_handshake(client);
    }

    return;
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

    // vector<SOCKET> connections;

    int conns = 0;

    while (1)
    {
        SOCKET client_socket = accept(server_listen, nullptr, nullptr);
        conns++;

        if (client_socket == SOCKET_ERROR)
        {
            cout << "Could not get client socket" << endl;
            continue;
        }

        // connections.push_back(client_socket);

        thread new_client(handle_client, client_socket, conns);
        new_client.join();
        conns--;
    }
    WSACleanup();
    return 0;
}