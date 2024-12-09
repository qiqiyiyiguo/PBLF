#include <winsock2.h>//导入必备头文件
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib")//链接静态库
int main()
{
    int err;
    char SendBuf[100];
    WSADATA wsaData;
    err = WSAStartup( MAKEWORD(2, 2), &wsaData);//指明socket版本
    
    if(!err){
        printf("socket start success\n");
    }else{
        printf("socket start fail\n");
        return -1;
    }

    SOCKET cliSocket=socket(AF_INET,SOCK_STREAM,0);//指明协议族、TCP连接

    if(cliSocket != -1){
        printf("create socket success\n");
    }
    
    
    SOCKADDR_IN client_addr;
    client_addr.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");//本机IP地址
    client_addr.sin_family=AF_INET;
    client_addr.sin_port=htons(6666);
    
   
    
    
    
    
    int r=connect(cliSocket,(SOCKADDR*)&client_addr,sizeof(SOCKADDR));

    

    while (1)//死循环
    {
        gets(SendBuf);

        if(strcmp(SendBuf,"quit")==0)
            break;
        send(cliSocket,SendBuf,strlen(SendBuf)+1,0);
 
       }
    
    closesocket(cliSocket);
    WSACleanup();
    return 0;
}