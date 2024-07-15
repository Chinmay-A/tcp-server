#define _WIN32_WINNT 0x0501
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFFER_LENGTH 512

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main()
{
    // initialize winsock
    WSADATA wsadata;
    int startup_check = WSAStartup(MAKEWORD(2, 2), &wsadata);

    if (startup_check != 0)
    {
        printf("Winsock startup failed with code: %d\n",startup_check);
        return 1;
    }

    struct addrinfo *result = nullptr;
    struct addrinfo *ptr = nullptr;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    int iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);

    if (iResult != 0)
    {
        printf("Could not get addrinfo with code: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    SOCKET server_listen=socket(result->ai_family,result->ai_socktype,result->ai_protocol);

    if(server_listen==INVALID_SOCKET){
        printf("Could not create Socket\n");
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    int bind_result=bind(server_listen,result->ai_addr,(int)result->ai_addrlen);
    
    if(bind_result==SOCKET_ERROR){
        printf("Failed to bind socket with error code: %d\n",bind_result);
        freeaddrinfo(result);
        closesocket(server_listen);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

 
        
    if(listen(server_listen,SOMAXCONN)==SOCKET_ERROR){
        printf("Could not listen on the created socket\n");
        closesocket(server_listen);
        WSACleanup();
        return 1;
    }

    SOCKET client=INVALID_SOCKET;

    client=accept(server_listen,NULL,NULL);
    if(client==INVALID_SOCKET){
        printf("Could not accept incoming connection\n");
        closesocket(server_listen);
        WSACleanup();
        return 1;
    }

    char incoming_buffer[DEFAULT_BUFFER_LENGTH];
    int incomingRes, outgoingRes;
    int incomingBufferLen=DEFAULT_BUFFER_LENGTH;

    do{
        incomingRes=recv(client,incoming_buffer,incomingBufferLen,0);

        if(incomingRes>0){
            outgoingRes=send(client,incoming_buffer,incomingRes,0);
            if(outgoingRes==SOCKET_ERROR){
                printf("Failed to deliver outgoing message\n");
                closesocket(client);
                WSACleanup();
                return 1;
            }
            printf("Bytes Sent: %d\n",outgoingRes);
        }
        else if(incomingRes==0){
            printf("Closing Connection\n");
        }
        else{
            printf("Failed to recieve data\n");
            closesocket(client);
            WSACleanup();
            return 1;
        }
    }
    while(incomingRes>0);

    return 0;
}
