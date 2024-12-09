#include <stdio.h>
#include <string.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == client_socket) {
        printf("create socket failed!!!\n");
        return -1;
    }

    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &target.sin_addr.s_addr);

    if (-1 == connect(client_socket, (struct sockaddr*)&target, sizeof(target))) {
        printf("connect server failed!!!\n");
        closesocket(client_socket);
        return -1;
    }

    while (1) {
        char sbuffer[1024] = { 0 };
        printf("please enter: ");
        scanf_s("%1023s", sbuffer, sizeof(sbuffer)); // 注意 scanf_s 的使用，指定缓冲区大小

        send(client_socket, sbuffer, strlen(sbuffer), 0);

        char rbuffer[1024] = { 0 };
        int recv_size = recv(client_socket, rbuffer, 1024, 0);
        if (recv_size <= 0) break;
        printf("%s\n", rbuffer);
    }

    closesocket(client_socket);
    WSACleanup(); // 清理 Winsock
    return 0;
}