#include <stdio.h>
#include <string.h>
#include <ws2tcpip.h>
#include <WinSock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

#define BUFFER_SIZE 1024

typedef struct {
    SOCKET socket;
} thread_data;

DWORD WINAPI SendThread(LPVOID param) {
    thread_data* data = (thread_data*)param;
    char client_message[BUFFER_SIZE] = { 0 };

    while (1) {
        printf("%llu: Please enter: ", data->socket);
        fgets(client_message, sizeof(client_message), stdin);

        if (strcmp(client_message, "exit\n") == 0) {
            break;
        }

        int sent = send(data->socket, client_message, (int)strlen(client_message), 0);
        if (sent == SOCKET_ERROR) {
            printf("Send message to server failed...\n");
            break;
        }
        else {
            printf("%llu: %s", data->socket, client_message);
        }
    }

    return 0;
}

DWORD WINAPI ReceiveThread(LPVOID param) {
    thread_data* data = (thread_data*)param;
    char server_message[BUFFER_SIZE] = { 0 };

    while (1) {
        int received = recv(data->socket, server_message, sizeof(server_message) - 1, 0);
        if (received == SOCKET_ERROR) {
            printf("Receive message from server failed...\n");
            break;
        }
        else if (received == 0) {
            printf("Server closed the connection.\n");
            break;
        }
        else {
            server_message[received] = '\0'; // Ensure null-terminated string
            printf("Server: %s\n", server_message);
        }
    }

    return 0;
}

int main() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
        return -1;
    }

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        printf("Create client socket failed...\nError code: %d", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(7777); // Port number must match the server
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr.s_addr); // Connect to local server

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Connect to server failed...\n");
        closesocket(client_socket);
        WSACleanup();
        return -1;
    }

    thread_data data = { client_socket };
    HANDLE sendThread = CreateThread(NULL, 0, SendThread, &data, 0, NULL);
    HANDLE receiveThread = CreateThread(NULL, 0, ReceiveThread, &data, 0, NULL);

    // Wait for threads to finish
    WaitForSingleObject(sendThread, INFINITE);
    WaitForSingleObject(receiveThread, INFINITE);

    // Cleanup
    closesocket(client_socket);
    WSACleanup();
    return 0;
}