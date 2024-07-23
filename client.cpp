#define _WIN32_WINNT 0x0501
#define DEFAULT_PORT "27026"
#define DEFAULT_BUFLEN 512

#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include<bits/stdc++.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main()
{

    WSADATA wsa_data;

    int init_check = WSAStartup(MAKEWORD(2, 2), &wsa_data);

    if (init_check != 0)
    {
        printf("Unable to Initialize winsock with code: %d\n", init_check);
        return 1;
    }

    struct addrinfo *result = nullptr;
    struct addrinfo *ptr = nullptr;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int addr_check = getaddrinfo("localhost", "27015", &hints, &result);

    if (addr_check != 0)
    {
        printf("getaddrinfo failed with code: %d\n", addr_check);
        WSACleanup();
        return 1;
    }

    SOCKET client = INVALID_SOCKET;

    ptr = result;

    client = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    if (client == INVALID_SOCKET)
    {
        printf("Could not create socket\n");
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    int conn_check = connect(client, ptr->ai_addr, (int)ptr->ai_addrlen);

    if (conn_check == SOCKET_ERROR)
    {
        closesocket(client);
        client = INVALID_SOCKET;
    }

    freeaddrinfo(result);

    if (client == INVALID_SOCKET)
    {
        printf("Unable to connect to server\n");
        WSACleanup();
        return 1;
    }

    int recvbuflen = DEFAULT_BUFLEN;

    const char *sendbuf = "Test message";
    char receive_buffer[DEFAULT_BUFLEN];

    int send_check = send(client, sendbuf, (int)strlen(sendbuf), 0);

    if (send_check == SOCKET_ERROR)
    {
        printf("Failed to send data to server\n");
        closesocket(client);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %d\n", send_check);

    int end_send = shutdown(client, SD_SEND);

    if (end_send == SOCKET_ERROR)
    {
        printf("Could not close send stream\n");
        closesocket(client);
        WSACleanup();
        return 1;
    }

    int rec_check;

    do
    {
        rec_check = recv(client, receive_buffer, recvbuflen, 0);
        if (rec_check > 0)
        {
            //printf("Bytes Received: %d\n", rec_check);
            string incoming_message(receive_buffer,rec_check);
            cout<<"Received Message: "<<incoming_message<<" ("<<rec_check<<" Bytes)"<<endl;
        }
        else if (rec_check == 0)
        {
            printf("Connection closed\n");
        }
        else
        {
            printf("Receive failed with error code: %d\n", WSAGetLastError());
        }
    } while (rec_check > 0);
}