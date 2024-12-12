#include <stdio.h>
#include <string.h>
#include <ws2tcpip.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")//windows ��ʹ�����繦����Ҫ��ʼ����Ȩ��
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
	server_addr.sin_port = htons(7777);						//�˿ں�Ҫ��server����һ��
	inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr.s_addr);		//ǰ�˽�����ʾ����client���棬��ʾ�������ܹ���IP��
	
	if (-1 == connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))) {		//connect��ֹbind�׽��ֺͺϲ��壬����������server����ʧ�ܣ�һ����server�ܾ����ӻ��߲�����
		//connect�ɹ������Ϊ�׽����غϣ����ɿ����ã�send��recv�����ڶɿڷ��ͻ��ǽ��ܴ�������Ѹ���Դ�ļ��д�������ݷ��ڽӿڴ����������߶Է����������ݣ����Ƕɿ���˫�е������ǵ��е�������˫��send��recv����Ե�
		printf("connect server failed...\n");
		closesocket(client_socket);
		return -1;
	}
	while (1) {
		char client_message[1024] = { 0 };
		char server_message[1024] = { 1 };
		printf("%llu:Please enter:",client_socket);

		fgets(client_message, sizeof(client_message), stdin);//����client_message
		
		if (strcmp(client_message, "exit\n") == 0) {
			break;
		}
		int set = send(client_socket, client_message, (int)strlen(client_message), 0);		//��client_message����client_socket��������server�Ƿ�����
		
		if (-1 == set) {
			printf("send message to Server failed...\n");
			closesocket(client_socket);			//�Ͽ�������
		}
		else {
			printf("%llu:%s\n", client_socket, client_message);
		}

		memset(server_message, 0, sizeof(server_message));
		int ret = recv(client_socket, server_message, sizeof(server_message) - 1, 0);		//�����ݴ�client_socket�ó������ŵ�server_message��λ�ã�ͬ��printf��scanf
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
