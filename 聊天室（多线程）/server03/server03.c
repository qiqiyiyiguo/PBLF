#include <stdio.h>
#include <string.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

DWORD WINAPI thread_func(LPVOID lpThreadParameter) {
    SOCKET client_socket = *(SOCKET*)lpThreadParameter;
    free(lpThreadParameter);
    while (1) {
        char client_message[1024] = { 0 };
        int ret = recv(client_socket, client_message, 1024, 0);
        if (ret <= 0)break;
        printf("%llu:%s\n", client_socket, client_message);

        char server_message[1024] = { 0 };
        printf("Server:Please enter:");
        scanf_s("%1023s", server_message, sizeof ( server_message));
        int set=send(client_socket, server_message, (int)strlen(server_message), 0);
        if (-1 == set) {
            printf("send message to Client failed...\n");
        }
        else {
            printf("Server:%s\n", server_message);
        }
    }
    printf("socket:%llu,disconnect.\n", client_socket);
    closesocket(client_socket);
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == server_socket) {
        printf("Create socket failed!\n");
        return -1;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(7777);
    inet_pton(AF_INET, "0.0.0.0", &server.sin_addr.s_addr);

    if (bind(server_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error: %d\n", WSAGetLastError());
        closesocket(server_socket);
        return -1;
    }

    if (listen(server_socket, 10) == SOCKET_ERROR) {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        closesocket(server_socket);
        return -1;
    }

    while (1) {
        SOCKET client_socket = accept(server_socket, NULL, NULL);
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
    closesocket(server_socket);
    WSACleanup();
    return 0;
}