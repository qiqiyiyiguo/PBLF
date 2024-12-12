#include <stdio.h>
#include <string.h>
#include <ws2tcpip.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib") // Link with ws2_32.lib

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
    server_addr.sin_port = htons(7777); // �˿ں�Ҫ�����������һ��
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr.s_addr); // ���ӵ����ط�����

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Connect to server failed...\n");
        closesocket(client_socket);
        WSACleanup();
        return -1;
    }

    char client_message[1024] = { 0 };
    char server_message[1024] = { 0 };
    printf("Connected to server. Enter messages (type 'exit' to quit):\n");

    while (1) {
        printf("%llu: Please enter: ", client_socket);
        fgets(client_message, sizeof(client_message), stdin); // ʹ��fgets����scanf_s

        if (strcmp(client_message, "exit\n") == 0) {
            break; // �˳�����
        }

        int sent = send(client_socket, client_message, (int)strlen(client_message), 0);
        if (sent == SOCKET_ERROR) {
            printf("Send message to server failed...\n");
            break;
        }
        else {
            printf("%llu: %s", client_socket, client_message);
        }

        memset(server_message, 0, sizeof(server_message)); // ��ս��ջ�����
        int received = recv(client_socket, server_message, sizeof(server_message) - 1, 0);
        if (received == SOCKET_ERROR) {
            printf("Receive message from server failed...\n");
            break;
        }
        else if (received == 0) {
            printf("Server closed the connection.\n");
            break;
        }
        else {
            server_message[received] = '\0'; // ȷ���ַ�����NULL��β
            printf("Server: %s\n", server_message);
        }
    }

    closesocket(client_socket);
    WSACleanup();
    return 0;
}