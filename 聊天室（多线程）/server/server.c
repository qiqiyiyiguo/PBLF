#include <stdio.h>
#include <string.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

DWORD WINAPI thread_func(LPVOID lpThreadParameter) {
    SOCKET client_socket = *(SOCKET*)lpThreadParameter;
    free(lpThreadParameter); // 释放分配的内存
    while (1) {
        char buffer[1024] = { 0 };
        int ret = recv(client_socket, buffer, 1024, 0);
        if (ret <= 0) break;
        printf("Received from socket %llu: %s\n", client_socket, buffer);
        send(client_socket, buffer, (int)strlen(buffer), 0);
    }
    printf("Socket %llu disconnected.\n", client_socket);
    closesocket(client_socket); // 关闭客户端套接字
    return 0;
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == listen_socket) {
        printf("Create socket failed!\n");
        return -1;
    }

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(8080);
    inet_pton(AF_INET, "0.0.0.0", &local.sin_addr.s_addr);

    if (bind(listen_socket, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR) {
        printf("Bind failed with error: %d\n", WSAGetLastError());
        closesocket(listen_socket);
        return -1;
    }

    if (listen(listen_socket, 10) == SOCKET_ERROR) {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        closesocket(listen_socket);
        return -1;
    }

    while (1) {
        SOCKET client_socket = accept(listen_socket, NULL, NULL);
        if (INVALID_SOCKET == client_socket) {
            printf("Accept failed with error: %d\n", WSAGetLastError());
            continue;
        }
        printf("New connection on socket %llu\n", client_socket);

        SOCKET* sockfd = (SOCKET*)malloc(sizeof(SOCKET));
        if (sockfd == NULL) {
            printf("Malloc failed\n");
            closesocket(client_socket);
            continue;
        }
        *sockfd = client_socket;
        HANDLE thread = CreateThread(NULL, 0, thread_func, sockfd, 0, NULL);
        if (thread == NULL) {
            printf("CreateThread failed with error: %d\n", GetLastError());
            closesocket(client_socket);
            free(sockfd);
            continue;
        }
        CloseHandle(thread); // 关闭线程句柄，如果不需要等待线程结束
    }

    // 这个循环是无限的，但为了完整性，程序结束前应该清理资源
    closesocket(listen_socket);
    WSACleanup();
    return 0;
}