#define _WIN32_WINNT _WIN32_WINNT_WIN10
#define PORT 27015

#include <iostream>
#include <bits/stdc++.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>

#include <thread>

#include "helper.cpp"
#include "filehandler.cpp"

#include <sys/stat.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

void send_code(SOCKET &client, char a, char b)
{

    string code = get_code(a - 'a', b - 'a');
    char send_buff[code.length()];

    fill_buffer(code, send_buff);

    int bytes_sent = send(client, send_buff, sizeof(send_buff), 0);

    cout << "Sent code: " << code << " (" << bytes_sent << " Bytes)" << endl;
}

void send_ok_handshake(SOCKET &client)
{

    char ok_buffer[] = "Hello World Server Test Deploy";
    int bytes_sent = send(client, ok_buffer, sizeof(ok_buffer), 0);
    cout << "Sent decoy message to client: " << bytes_sent << " Bytes" << endl;
    return;
}

void handle_with_identifier(SOCKET client, string message, map<string, int> &m)
{
    int n = message.length();
    string identifier = message.substr(1, n - 1);

    cout << "In identifier with identifier size: " << n - 1 << endl;

    if (n - 1 != 3)
    {
        return;
    }

    if (m[identifier] >= 10)
    {
        cout << "Resource Limit Reached for user with identifier: " << identifier << endl;
        return;
    }
    m[identifier]++;
    identifier = identifier + to_string(m[identifier]);

    char send_buff[identifier.length()];

    fill_buffer(identifier, send_buff);

    int bytes_sent = send(client, send_buff, sizeof(send_buff), 0);

    if (bytes_sent <= 0)
    {
        cout << "Identifier could not be sent, client should retry" << endl;
        return;
    }

    cout << "Sent Identifier: " << identifier << " (" << identifier.length() << ")" << endl;
    return;
}

void send_error(SOCKET client)
{

    string error_msg_str = "Invalid Request, please generate identifier before hand";
    char error_msg[error_msg_str.length()];
    fill_buffer(error_msg_str, error_msg);
    send(client, error_msg, sizeof(error_msg), 0);
    return;
}

void add_resource(SOCKET client, string message, map<string, int> &m)
{
    int n = message.length();

    string identifier = message.substr(1, 3);
    string file_name = message.substr(1, 4);

    if (m.find(identifier) == m.end())
    {
        send_error(client);
        return;
    }

    string resource_contents = message.substr(4, n - 5);

    string new_file = write_to_txt(resource_contents, file_name);

    string confirm_str = "OK";
    char confirm[confirm_str.length()];
    fill_buffer(confirm_str, confirm);
    int bytes_sent = send(client, confirm, sizeof(confirm), 0);

    return;
}

void handle_handshake(SOCKET client)
{
    string send_handshake_str = "Hello, from the remote file repository, the server is active";
    char send_handshake[send_handshake_str.length()];
    fill_buffer(send_handshake_str, send_handshake);
    send(client, send_handshake, sizeof(send_handshake), 0);
    return;
}

void fetch_resource(SOCKET client, string message)
{
    int n = message.length();

    string file_name_str = message.substr(1, n - 1);

    string file_path = "./resources/" + file_name_str + ".txt";

    string resource = get_from_txt(file_path);
    char send_resource[resource.length()];
    fill_buffer(resource, send_resource);

    int bytes_sent = send(client, send_resource, sizeof(send_resource), 0);

    if (bytes_sent < 0)
    {
        cout << "Failed to fetch resource, the client must retry" << endl;
        return;
    }

    cout << "Size of Resource: " << sizeof(resource) << "Sent " << bytes_sent << " Bytes" << endl;
    return;
}

void handle_client(SOCKET client, int &i, queue<string> &q, map<string, int> &m, int &active)
{
    cout << "New connection established, active connections: " << i << endl;

    char recbuff[1024];

    int bytes_recvd = recv(client, recbuff, sizeof(recbuff), 0);

    if (bytes_recvd <= 0)
    {
        i--;
        cout << "Invalid Request, active connections: " << i << endl;
        return;
    }

    string message_recvd(recbuff, bytes_recvd);

    cout << "Message from Client: " << message_recvd << endl;

    if (message_recvd[0] == 'i')
    {
        handle_with_identifier(client, message_recvd, m);
    }
    else if (message_recvd[0] == 'm')
    {
        add_resource(client, message_recvd, m);
    }
    else if (message_recvd[0] == 'r')
    {
        cout << "Received Message: " << message_recvd << endl;
        fetch_resource(client, message_recvd);
    }
    else if (message_recvd[0] == 'q')
    {
        cout << "Received Message: " << message_recvd << endl;
        // signal for server to quit: q1xf0e
        if (bytes_recvd = 6 && message_recvd[1] == '1' && message_recvd[2] == 'x' && message_recvd[3] == 'f' && message_recvd[4] == '0' && message_recvd[5] == 'e')
        {
            active = 0;
            return;
        }
        else
        {
            handle_handshake(client);
        }
    }
    else
    {
        handle_handshake(client);
    }

    closesocket(client);
    i--;
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

    int conns = 0;

    queue<string> q;
    map<string, int> m;

    int active = 1;

    while (1)
    {
        if (!active)
        {
            break;
        }

        SOCKET client_socket = accept(server_listen, nullptr, nullptr);
        conns++;
        if (!active)
        {
            break;
        }

        if (client_socket == SOCKET_ERROR)
        {
            cout << "Could not get client socket" << endl;
            continue;
        }

        // connections.push_back(client_socket);
        if (!active)
        {
            break;
        }

        thread new_client(handle_client, client_socket, ref(conns), ref(q), ref(m), ref(active));
        new_client.detach();
    }

    cout << "Closing Server: " << endl;
    WSACleanup();
    return 0;
}