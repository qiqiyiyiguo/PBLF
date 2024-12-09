#include <winsock2.h>//导入必备头文件
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib")//链接静态库
int main()
{
  
    WSADATA wsaData;
    WSAStartup( MAKEWORD(2, 2), &wsaData);//指明socket版本
    
    if(WSAStartup == 0){
        printf("socket start success\n");
    }else{
        printf("socket start fail\n");
    }

    SOCKET serSocket=socket(AF_INET,SOCK_STREAM,0);//指明协议族、TCP连接

    if(serSocket != -1){
        printf("create socket success\n");
    }
    
    struct sockaddr_in serv_addr;
    SOCKADDR_IN addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);//本机IP地址
    addr.sin_port=htons(6666);
    bind(serSocket,(SOCKADDR*)&addr,sizeof(SOCKADDR));
    

    listen(serSocket,5);
    

    printf("waiting...\n");

    SOCKADDR_IN clientsocket;
    int len=sizeof(SOCKADDR);

    SOCKET aID=accept(serSocket,(SOCKADDR*)&clientsocket,&len);

    if(aID){
        printf("connet to client\n");
    }

    while (1)//死循环
    {
        char sendBuf[100];//发送
        sprintf(sendBuf,"client IP %s",inet_ntoa(clientsocket.sin_addr));//输出点分十进制的IP
        send(aID,sendBuf,strlen(sendBuf)+1,0);//发送数据

        char receiveBuf[100];//接收
        int RecvLen;
        RecvLen=recv(aID,receiveBuf,100,0);

        if(RecvLen!=-1)
            printf("%d %s\n",RecvLen,receiveBuf);
        else
            break;
 
       }
    
    closesocket(aID);
    WSACleanup();
    return 0;
}

