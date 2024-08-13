#define _WIN32_WINNT _WIN32_WINNT_WIN10
#define DESTPORT 27015

#include <iostream>
#include <bits/stdc++.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>

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
    int check = inet_pton(AF_INET, "172.23.66.214", &curr_addr.sin_addr);
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

string get_code(int a, int b)
{
    string secret_code = "XG452552" + to_string(a + b) + "00065" + to_string(abs(a - b)) + "f0xE";
    return secret_code;
}
