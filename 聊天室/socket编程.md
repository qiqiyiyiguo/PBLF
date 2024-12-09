# socket基础知识

1. socket就是套接字，规定了程序请求因特网向另一个端上的程序交付数据的方式

2. 流式套接字对应TCP协议

   数据报套接字对应UCP协议

3. 流：实现数据交换和传输的载体

4. 阻塞调用：调用结果返回前当前线程会被挂起

   非阻塞调用：不能立刻得到结果之前，该调用不会阻塞当前线程

5. 同步：发出一个功能调用时，在没有得到结果之前，该调用不返回，同时其它线程不能调用此方法

   异步：发出一个功能调用不论结果不影响任务执行

6. 带外数据：逻辑上独立的传输通道，独立于普通数据传送给用户

7. 有关TCP/UCP笔记见*计网基础.md*文件



# 字节序

## 概念

大于一个字节类型的数据在内存中的存放顺序

## 分类

大端（网络字节序）：高位字节放在内存低地址端

小端（主机字节序）：高位字节放在内存高地址端

## 地址

内存地址的分配从小到大增长

## 字节

靠左边是高位，靠右边是低位

# 编写过程

## 前置准备

根据教程在tasks.json文件中args字段添加新的参数

## 服务器端

#### WSAStartup

1. 需实现包含对应的头文件及静态库

2. `#pragma comment(lib,"ws2_32.lib") `表示链接必备静态库

3. 用于初始化socket编程，指明Windows系统中socket（）版本

4. 成功返回0，失败返回-1

5. 代码：

   ```java
   WSADATA wsaData;
   //第一个2是主版本号，第二个2是服版本号
   //MAKEWORD()宏函数用于对版本号进行格式转换
   //&wasData是指向WSAData结构体的指针
   WSAStartup( MAKEWORD(2, 2), &wsaData); 
   ```

#### socket

1. 功能：创建套接字
2. 头文件：\#include <winsock2.h>
3. 成功返回套接字的文件描述符（非负值），失败返回-1（没有执行WSAStartup初始化）
4. 原型：`int socket( int af, int type, int protocol);`
5. 代码：`int sockfd=socket(AF_INET,SOCK_STREAM,0);//TCP`
6. af：指明协议族
7. type：socket类型，SOCK_STREAM对应TCP
8. protocol指明传输协议，一般取0

#### bind

1. 功能：将监听套接字绑定到本地地址和端口上

2. 头文件：\#include <winsock2.h>

3. 成功返回非负值，失败返回-1

4. 原型`int bind(int sockfd, const struct sockaddr_in *addr, int addrlen);`

5. sockfd:socket的返回值

6. addr：sockaddr结构体变量的指针

7. addrlen：addr变量的大小

8. 需先创建结构体变量sockaddr_in、serv_addr

   可用`struct sockaddr_in serv_addr`

9. srveraddr中的参数sin_family指代函数族，取值应与af相同

   `servaddr.sin_family=AF_INET; `

10. 我们的程序尽量在1024-65536之间分配端口号

    `servaddr.sin_port=htons(2000); `

    此处2000即为端口号

11. 将本机IP地址转换为十进制

    `serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1"); `

12. 绑定套接字

    `bind(sockfd,(sockaddr*)&servaddr,sizeof(servaddr)); `

#### listen

1. 功能：让套接字进入被动监听状态

2. 头文件同上

3. 成功返回0，失败返回-1

4. 原型：

   `int listen(int sockfd, int queue_length);`

5. sockfd：套接字的描述符

6. queue_length：指定在sever程序调用accept函数前最大允许进入的连接请求数，一般取5

#### accept

1. 功能：接收客户端连接请求

2. 头文件同上

3. 成功返回非负值，失败返回-1

4. 原型：

   `int accept(int sockfd, struct sockaddr *addr, int *addrlen);`

5. sockfd：套接字描述符

6. addr：sockaddr结构体变量的指针

7. addrlen：addr变量的大小

8. 需要先创建客户端地址结构体

   `struct sockaddr_in clientaddr`

9. 创建整型变量接受accept函数的返回值

   `int aID`

10. `aID=accept(sockfd,(sockaddr*)&clientaddr,&sizeof( clientaddr));`

#### recv

1. 功能：接受客户端或服务端传来的数据

2. 头文件同上

3. 成功返回值为接收到的数据长度，等于零没有接受到数据，小于零传输失败socket报错

4. 首先定义字符串用来保存客户端发来的数据

   `charrecBuf[200];`

5. 实例：

   `recv(aID,recBuf,200,0);`

6. aID：accept的返回值，表示连接成功的套接字描述符

   *在客户端是connet的返回值*

7. buf：要接收的数据所在的缓冲区地址，即一个空的字符数组的首地址

8. len：要接收数据的字节数

9. flags：附带标记，一般为0

   1. MSG_DONTROUTE：表示不使用指定路由，对send、sendto有效 
   2. MSG_PEEK：对recv, recvfrom有效，表示读出网络数据后不清除已读的数据 
   3. MSG_OOB：对发送接收都有效，表示发送或接受加急数据

#### send

1. 功能：发送数据

2. 头文件同上

3. 返回值小于零报错，等于零对方关闭了连接，大于零为发送的数据长度

4. 原型：

   `int send(int aID, const char *buf, int len, int flags);`

5. 需先定义一个数组保存发送的数据

   `charsendBuf[200];`

6. aID、buf、flags与recv中的定义相同

7. len：要发送 的数据的实际字节数+1

8. 实例：

   `send(aID,sendBuf,strlen(sendBuf)+1,0);`

#### closesocket

1. 功能：关闭套接字
2. 头文件同上
3. `closesocket(aID);`
4. aID是接收客户端请求的返回值，定义同上

#### WSACleanup

1. 功能：终止对socket字库的使用

2. 成功返回0

3. 代码：

   `WSACleanup();`

## 客户端

#### socket

同服务端

#### connect

1. 功能：向服务端发起连接请求

2. 头文件同上

3. 成功返回0，失败返回-1

4. 原型：

   `int connect(int sockcd, const struct sockaddr *addr, int addrlen);`

5. 各变量定义与上文相同

#### send

同服务端

注意：aID是connect的返回值

#### recv

同服务端

#### closesocket

同服务端

# 遇到的其它问题

1. 客户端无法连接到服务器端-IP地址错误，更改后成功

2. 终端显示中文乱码-未解决，全部改为英文

3. 不同教程中导入的包、用的代码格式很不一样，我看了几版不同的教程并模仿它们的代码，这一版是最简单的，也是我模仿成功的一版

   参考教程链接[Socket编程入门 - Socket通信技术简明教程 - C语言网 (dotcpp.com)](https://www.dotcpp.com/course/socket/#:~:text=Socket编程入门 - Socket通信技术简明教程,点击打开 在线编译器，边学边练 本教程为Socket套接字编程教程，即使用Socket套接字实现两台机器时间的网络通信，可以实现数据的传输等功能，实现基本的、简单的类似”聊天“功能。 本教程需要C、C%2B%2B语言基础，简单的TCP%2FIP协议、IP地址、端口相关的常识知识即可，其更多介绍会在本教程中讲解，本套教程用的demo程序采用windows平台C语言实现，更提供了TCP协议下的服务端和客户端的完整通信代码。)

   [Socket编程（C语言实现）——TCP协议（网络间通信AF_INET）的流式（SOCK_STREAM）+报式（SOCK_DGRAM）传输【多线程+循环监听】_tcp spcket c-CSDN博客](https://blog.csdn.net/weixin_42167759/article/details/81255812)


# 多用户数据传输任务遇到的部分重大问题及解决办法

1. linux系统与Windows系统所用代码有所不同，导入的包就不同，相关函数也有所区别，例如不需要WSAStartup函数，相应也不需要关闭函数等

   解决：查询资料，参照实例代码按照linux系统的要求结合在Windows系统下编写的框架重写客户端代码

   部分参考链接：

   [Linux网络编程之Tcp（内含服务器、客户端代码实例）_linux 下网络编程 tcp 发送,接收,客户端代码实例-CSDN博客](https://blog.csdn.net/QQ1402369668/article/details/86090092#:~:text=Linux下的soc)

   [socket在windows下和linux下的区别与移植_wsagetlasterror移植-CSDN博客](https://blog.csdn.net/xiaomengdiemeng/article/details/116675005)

2. 主机与wsl无法通信，查询资料后尝试重新绑定IP地址

   [如何查看自己windows电脑的ip地址_windows本机ip-CSDN博客](https://blog.csdn.net/qq_57828911/article/details/131605529#:~:text=在Win11系统,的IPv4地址。)

3. 客户端输入字符后无法传输到服务器端

   [linux下的strcmp（）、strcpy（）、strlen（）、strcat（）、strncat（）函数_linux strcmp-CSDN博客](https://blog.csdn.net/qwerwqwe/article/details/127374254)

   [fgets()函数的详解-使用技巧-C语言基础_fgets函数用法-CSDN博客](https://blog.csdn.net/Devour_/article/details/110955333#:~:text=fgets()（函数)

