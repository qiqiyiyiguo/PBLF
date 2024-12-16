#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

typedef struct {
    SOCKET socket;
    CRITICAL_SECTION cs; // 用于同步的临界区
    char sendBuffer[BUFFER_SIZE]; // 发送缓冲区
    BOOL sendPending; // 是否有待发送数据
} thread_data;

// 客户端列表
SOCKET client_list[MAX_CLIENTS];
thread_data client_data[MAX_CLIENTS]; // 每个客户端的线程数据
int client_count = 0;

// 临界区对象，用于同步对客户端列表的访问
CRITICAL_SECTION clientListCS;

DWORD WINAPI SendThread(LPVOID param) {
    thread_data* data = (thread_data*)param;

    while (1) {
        EnterCriticalSection(&data->cs); // 进入临界区
        if (data->sendPending) {
            int sent = send(data->socket, data->sendBuffer, (int)strlen(data->sendBuffer), 0);
            if (sent == SOCKET_ERROR) {
                printf("Send message to client failed...\n");
            }
            else {
                printf("Message sent: %s\n", data->sendBuffer);
            }
            memset(data->sendBuffer, 0, BUFFER_SIZE); // 清空发送缓冲区
            data->sendPending = FALSE; // 重置发送标志
        }
        LeaveCriticalSection(&data->cs); // 离开临界区
        Sleep(100); // 避免CPU占用过高
    }
    return 0;
}

DWORD WINAPI ReceiveThread(LPVOID param) {
    thread_data* data = (thread_data*)param;
    char server_message[BUFFER_SIZE] = { 0 };

    while (1) {
        int received = recv(data->socket, server_message, sizeof(server_message) - 1, 0);
        if (received == SOCKET_ERROR) {
            printf("Receive message from client failed...\n");
            break;
        }
        else if (received == 0) {
            printf("Client closed the connection.\n");
            break;
        }
        else {
            server_message[received] = '\0'; // Ensure null-terminated string
            printf("Client: %s\n", server_message);
        }
    }
    return 0;
}

void SendToClient(int client_index, const char* message) {
    if (client_index >= 0 && client_index < client_count) {
        thread_data* data = &client_data[client_index];
        EnterCriticalSection(&data->cs); // 进入临界区
        strncpy_s(data->sendBuffer, BUFFER_SIZE, message, BUFFER_SIZE - 1);
        data->sendBuffer[BUFFER_SIZE - 1] = '\0'; // Ensure null-terminated string
        data->sendPending = TRUE;
        LeaveCriticalSection(&data->cs); // 离开临界区
    }
}

void ProcessCommands() {
    char command[256];
    char message[BUFFER_SIZE];
    int client_index;
    char* context = NULL;

    while (1) {
        printf("Enter command (send <client_index> <message> or exit): ");
        fflush(stdout); // 确保命令行提示符被立即打印
        fgets(command, sizeof(command), stdin);
        if (strcmp(command, "exit\n") == 0) {
            break;
        }

        char* token = strtok_s(command, " ", &context);
        if (strcmp(token, "send") == 0) {
            token = strtok_s(NULL, " ", &context);
            client_index = atoi(token);
            token = strtok_s(NULL, "\n", &context);
            if (client_index >= 0 && client_index < client_count && token != NULL) {
                strncpy_s(message, BUFFER_SIZE, token, BUFFER_SIZE - 1);
                message[BUFFER_SIZE - 1] = '\0'; // Ensure null-terminated string
                SendToClient(client_index, message);
            }
            else {
                printf("Invalid client index.\n");
            }
        }
    }
}

DWORD WINAPI thread_func(LPVOID lpThreadParameter) {
    SOCKET client_socket = *(SOCKET*)lpThreadParameter;
    free(lpThreadParameter);

    int client_index;
    EnterCriticalSection(&clientListCS); // 进入临界区
    for (client_index = 0; client_index < client_count; ++client_index) {
        if (client_list[client_index] == INVALID_SOCKET) {
            client_list[client_index] = client_socket;
            break;
        }
    }
    if (client_index == client_count) {
        client_list[client_count] = client_socket;
        client_count++;
    }
    LeaveCriticalSection(&clientListCS); // 离开临界区

    thread_data* data = &client_data[client_index];
    InitializeCriticalSection(&data->cs);
    data->socket = client_socket;
    data->sendPending = FALSE;
    strcpy_s(data->sendBuffer, BUFFER_SIZE, "");

    HANDLE sendThread = CreateThread(NULL, 0, SendThread, data, 0, NULL);
    HANDLE receiveThread = CreateThread(NULL, 0, ReceiveThread, data, 0, NULL);

    // Wait for threads to finish
    WaitForSingleObject(sendThread, INFINITE);
    WaitForSingleObject(receiveThread, INFINITE);

    // Cleanup
    closesocket(client_socket);
    DeleteCriticalSection(&data->cs);
    return 0;
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    InitializeCriticalSection(&clientListCS); // 初始化临界区

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

    HANDLE commandThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ProcessCommands, NULL, 0, NULL);
    if (commandThread == NULL) {
        printf("Create command thread failed with error: %d\n", GetLastError());
        closesocket(server_socket);
        WSACleanup();
        return -1;
    }
    CloseHandle(commandThread); // 关闭命令线程句柄

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
        CloseHandle(thread); // 关闭线程句柄
    }

    // 这个循环是无限的，但为了完整性，程序结束前应该清理资源
    closesocket(server_socket);
    WSACleanup();
    DeleteCriticalSection(&clientListCS); // 删除临界区
    return 0;
}