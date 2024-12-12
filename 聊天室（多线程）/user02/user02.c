#include <stdio.h>
#include <string.h>
#include <ws2tcpip.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")//windows 上使用网络功能需要开始网络权限
int main() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		printf("WSAStartup failed: %d\n", result);
		return -1;
	}

	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == client_socket) {
		printf("create client_socket failed...\nerror code:%d", GetLastError());
		WSACleanup();
		return -1;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(7777);						//端口号要与server保持一致
	inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr.s_addr);		//前端界面显示的是client界面，演示机都接受公网IP：
	
	if (-1 == connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))) {		//connect不止bind套接字和合并体，还申请连接server，若失败，一般是server拒绝连接或者不在线
		//connect成功后，理解为套接字重合，即渡口作用，send和recv都是在渡口发送或是接受传输物，即把各自源文件中储存的数据放在接口处，或者拿走对方送来的数据，但是渡口是双行道，不是单行道，所以双边send和recv是相对的
		printf("connect server failed...\n");
		closesocket(client_socket);
		return -1;
	}
	while (1) {
		char client_message[1024] = { 0 };
		char server_message[1024] = { 1 };
		printf("%llu:Please enter:",client_socket);

		fgets(client_message, sizeof(client_message), stdin);//输入client_message
		
		if (strcmp(client_message, "exit\n") == 0) {
			break;
		}
		int set = send(client_socket, client_message, (int)strlen(client_message), 0);		//将client_message放在client_socket，不考虑server是否拿走
		
		if (-1 == set) {
			printf("send message to Server failed...\n");
			closesocket(client_socket);			//断开连接了
		}
		else {
			printf("%llu:%s\n", client_socket, client_message);
		}

		memset(server_message, 0, sizeof(server_message));
		int ret = recv(client_socket, server_message, sizeof(server_message) - 1, 0);		//将数据从client_socket拿出来，放到server_message的位置（同理printf和scanf
		if (-1 == ret) {
			printf("receive messgage from server failed...\n");
		}
		else if (0 == ret) {
			printf("Server closed the connection.\n");
		}else{
			server_message[ret] = '\0';
			printf("Server:%s\n", server_message);
		}
	}

}
