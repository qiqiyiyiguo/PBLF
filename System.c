#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*
  Copyright (c) 2009-2017 Dave Gamble and cJSON contributors

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef cJSON__h
#define cJSON__h

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__

/* When compiling for windows, we specify a specific calling convention to avoid issues where we are being called from a project with a different default calling convention.  For windows you have 3 define options:

CJSON_HIDE_SYMBOLS - Define this in the case where you don't want to ever dllexport symbols
CJSON_EXPORT_SYMBOLS - Define this on library build when you want to dllexport symbols (default)
CJSON_IMPORT_SYMBOLS - Define this if you want to dllimport symbol

For *nix builds that support visibility attribute, you can define similar behavior by

setting default visibility to hidden by adding
-fvisibility=hidden (for gcc)
or
-xldscope=hidden (for sun cc)
to CFLAGS

then using the CJSON_API_VISIBILITY flag to "export" the same symbols the way CJSON_EXPORT_SYMBOLS does

*/

#define CJSON_CDECL __cdecl
#define CJSON_STDCALL __stdcall

/* export symbols by default, this is necessary for copy pasting the C and header file */
#if !defined(CJSON_HIDE_SYMBOLS) && !defined(CJSON_IMPORT_SYMBOLS) && !defined(CJSON_EXPORT_SYMBOLS)
#define CJSON_EXPORT_SYMBOLS
#endif

#if defined(CJSON_HIDE_SYMBOLS)
#define CJSON_PUBLIC(type)   type CJSON_STDCALL
#elif defined(CJSON_EXPORT_SYMBOLS)
#define CJSON_PUBLIC(type)   __declspec(dllexport) type CJSON_STDCALL
#elif defined(CJSON_IMPORT_SYMBOLS)
#define CJSON_PUBLIC(type)   __declspec(dllimport) type CJSON_STDCALL
#endif
#else /* !__WINDOWS__ */
#define CJSON_CDECL
#define CJSON_STDCALL

#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined (__SUNPRO_C)) && defined(CJSON_API_VISIBILITY)
#define CJSON_PUBLIC(type)   __attribute__((visibility("default"))) type
#else
#define CJSON_PUBLIC(type) type
#endif
#endif

/* project version */
#define CJSON_VERSION_MAJOR 1
#define CJSON_VERSION_MINOR 7
#define CJSON_VERSION_PATCH 18

#include <stddef.h>

/* cJSON Types: */
#define cJSON_Invalid (0)
#define cJSON_False  (1 << 0)
#define cJSON_True   (1 << 1)
#define cJSON_NULL   (1 << 2)
#define cJSON_Number (1 << 3)
#define cJSON_String (1 << 4)
#define cJSON_Array  (1 << 5)
#define cJSON_Object (1 << 6)
#define cJSON_Raw    (1 << 7) /* raw json */

#define cJSON_IsReference 256
#define cJSON_StringIsConst 512

/* The cJSON structure: */
typedef struct cJSON
{
    /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
    struct cJSON *next;
    struct cJSON *prev;
    /* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */
    struct cJSON *child;

    /* The type of the item, as above. */
    int type;

    /* The item's string, if type==cJSON_String  and type == cJSON_Raw */
    char *valuestring;
    /* writing to valueint is DEPRECATED, use cJSON_SetNumberValue instead */
    int valueint;
    /* The item's number, if type==cJSON_Number */
    double valuedouble;

    /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
    char *string;
} cJSON;

typedef struct cJSON_Hooks
{
      /* malloc/free are CDECL on Windows regardless of the default calling convention of the compiler, so ensure the hooks allow passing those functions directly. */
      void *(CJSON_CDECL *malloc_fn)(size_t sz);
      void (CJSON_CDECL *free_fn)(void *ptr);
} cJSON_Hooks;

typedef int cJSON_bool;

/* Limits how deeply nested arrays/objects can be before cJSON rejects to parse them.
 * This is to prevent stack overflows. */
#ifndef CJSON_NESTING_LIMIT
#define CJSON_NESTING_LIMIT 1000
#endif

/* Limits the length of circular references can be before cJSON rejects to parse them.
 * This is to prevent stack overflows. */
#ifndef CJSON_CIRCULAR_LIMIT
#define CJSON_CIRCULAR_LIMIT 10000
#endif

/* returns the version of cJSON as a string */
CJSON_PUBLIC(const char*) cJSON_Version(void);

/* Supply malloc, realloc and free functions to cJSON */
CJSON_PUBLIC(void) cJSON_InitHooks(cJSON_Hooks* hooks);

/* Memory Management: the caller is always responsible to free the results from all variants of cJSON_Parse (with cJSON_Delete) and cJSON_Print (with stdlib free, cJSON_Hooks.free_fn, or cJSON_free as appropriate). The exception is cJSON_PrintPreallocated, where the caller has full responsibility of the buffer. */
/* Supply a block of JSON, and this returns a cJSON object you can interrogate. */
CJSON_PUBLIC(cJSON *) cJSON_Parse(const char *value);
CJSON_PUBLIC(cJSON *) cJSON_ParseWithLength(const char *value, size_t buffer_length);
/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed. */
/* If you supply a ptr in return_parse_end and parsing fails, then return_parse_end will contain a pointer to the error so will match cJSON_GetErrorPtr(). */
CJSON_PUBLIC(cJSON *) cJSON_ParseWithOpts(const char *value, const char **return_parse_end, cJSON_bool require_null_terminated);
CJSON_PUBLIC(cJSON *) cJSON_ParseWithLengthOpts(const char *value, size_t buffer_length, const char **return_parse_end, cJSON_bool require_null_terminated);

/* Render a cJSON entity to text for transfer/storage. */
CJSON_PUBLIC(char *) cJSON_Print(const cJSON *item);
/* Render a cJSON entity to text for transfer/storage without any formatting. */
CJSON_PUBLIC(char *) cJSON_PrintUnformatted(const cJSON *item);
/* Render a cJSON entity to text using a buffered strategy. prebuffer is a guess at the final size. guessing well reduces reallocation. fmt=0 gives unformatted, =1 gives formatted */
CJSON_PUBLIC(char *) cJSON_PrintBuffered(const cJSON *item, int prebuffer, cJSON_bool fmt);
/* Render a cJSON entity to text using a buffer already allocated in memory with given length. Returns 1 on success and 0 on failure. */
/* NOTE: cJSON is not always 100% accurate in estimating how much memory it will use, so to be safe allocate 5 bytes more than you actually need */
CJSON_PUBLIC(cJSON_bool) cJSON_PrintPreallocated(cJSON *item, char *buffer, const int length, const cJSON_bool format);
/* Delete a cJSON entity and all subentities. */
CJSON_PUBLIC(void) cJSON_Delete(cJSON *item);

/* Returns the number of items in an array (or object). */
CJSON_PUBLIC(int) cJSON_GetArraySize(const cJSON *array);
/* Retrieve item number "index" from array "array". Returns NULL if unsuccessful. */
CJSON_PUBLIC(cJSON *) cJSON_GetArrayItem(const cJSON *array, int index);
/* Get item "string" from object. Case insensitive. */
CJSON_PUBLIC(cJSON *) cJSON_GetObjectItem(const cJSON * const object, const char * const string);
CJSON_PUBLIC(cJSON *) cJSON_GetObjectItemCaseSensitive(const cJSON * const object, const char * const string);
CJSON_PUBLIC(cJSON_bool) cJSON_HasObjectItem(const cJSON *object, const char *string);
/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when cJSON_Parse() returns 0. 0 when cJSON_Parse() succeeds. */
CJSON_PUBLIC(const char *) cJSON_GetErrorPtr(void);

/* Check item type and return its value */
CJSON_PUBLIC(char *) cJSON_GetStringValue(const cJSON * const item);
CJSON_PUBLIC(double) cJSON_GetNumberValue(const cJSON * const item);

/* These functions check the type of an item */
CJSON_PUBLIC(cJSON_bool) cJSON_IsInvalid(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsFalse(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsTrue(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsBool(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsNull(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsNumber(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsString(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsArray(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsObject(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsRaw(const cJSON * const item);

/* These calls create a cJSON item of the appropriate type. */
CJSON_PUBLIC(cJSON *) cJSON_CreateNull(void);
CJSON_PUBLIC(cJSON *) cJSON_CreateTrue(void);
CJSON_PUBLIC(cJSON *) cJSON_CreateFalse(void);
CJSON_PUBLIC(cJSON *) cJSON_CreateBool(cJSON_bool boolean);
CJSON_PUBLIC(cJSON *) cJSON_CreateNumber(double num);
CJSON_PUBLIC(cJSON *) cJSON_CreateString(const char *string);
/* raw json */
CJSON_PUBLIC(cJSON *) cJSON_CreateRaw(const char *raw);
CJSON_PUBLIC(cJSON *) cJSON_CreateArray(void);
CJSON_PUBLIC(cJSON *) cJSON_CreateObject(void);

/* Create a string where valuestring references a string so
 * it will not be freed by cJSON_Delete */
CJSON_PUBLIC(cJSON *) cJSON_CreateStringReference(const char *string);
/* Create an object/array that only references it's elements so
 * they will not be freed by cJSON_Delete */
CJSON_PUBLIC(cJSON *) cJSON_CreateObjectReference(const cJSON *child);
CJSON_PUBLIC(cJSON *) cJSON_CreateArrayReference(const cJSON *child);

/* These utilities create an Array of count items.
 * The parameter count cannot be greater than the number of elements in the number array, otherwise array access will be out of bounds.*/
CJSON_PUBLIC(cJSON *) cJSON_CreateIntArray(const int *numbers, int count);
CJSON_PUBLIC(cJSON *) cJSON_CreateFloatArray(const float *numbers, int count);
CJSON_PUBLIC(cJSON *) cJSON_CreateDoubleArray(const double *numbers, int count);
CJSON_PUBLIC(cJSON *) cJSON_CreateStringArray(const char *const *strings, int count);

/* Append item to the specified array/object. */
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToArray(cJSON *array, cJSON *item);
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item);
/* Use this when string is definitely const (i.e. a literal, or as good as), and will definitely survive the cJSON object.
 * WARNING: When this function was used, make sure to always check that (item->type & cJSON_StringIsConst) is zero before
 * writing to `item->string` */
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToObjectCS(cJSON *object, const char *string, cJSON *item);
/* Append reference to item to the specified array/object. Use this when you want to add an existing cJSON to a new cJSON, but don't want to corrupt your existing cJSON. */
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemReferenceToArray(cJSON *array, cJSON *item);
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemReferenceToObject(cJSON *object, const char *string, cJSON *item);

/* Remove/Detach items from Arrays/Objects. */
CJSON_PUBLIC(cJSON *) cJSON_DetachItemViaPointer(cJSON *parent, cJSON * const item);
CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromArray(cJSON *array, int which);
CJSON_PUBLIC(void) cJSON_DeleteItemFromArray(cJSON *array, int which);
CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromObject(cJSON *object, const char *string);
CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromObjectCaseSensitive(cJSON *object, const char *string);
CJSON_PUBLIC(void) cJSON_DeleteItemFromObject(cJSON *object, const char *string);
CJSON_PUBLIC(void) cJSON_DeleteItemFromObjectCaseSensitive(cJSON *object, const char *string);

/* Update array items. */
CJSON_PUBLIC(cJSON_bool) cJSON_InsertItemInArray(cJSON *array, int which, cJSON *newitem); /* Shifts pre-existing items to the right. */
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemViaPointer(cJSON * const parent, cJSON * const item, cJSON * replacement);
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInArray(cJSON *array, int which, cJSON *newitem);
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInObject(cJSON *object,const char *string,cJSON *newitem);
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInObjectCaseSensitive(cJSON *object,const char *string,cJSON *newitem);

/* Duplicate a cJSON item */
CJSON_PUBLIC(cJSON *) cJSON_Duplicate(const cJSON *item, cJSON_bool recurse);
/* Duplicate will create a new, identical cJSON item to the one you pass, in new memory that will
 * need to be released. With recurse!=0, it will duplicate any children connected to the item.
 * The item->next and ->prev pointers are always zero on return from Duplicate. */
/* Recursively compare two cJSON items for equality. If either a or b is NULL or invalid, they will be considered unequal.
 * case_sensitive determines if object keys are treated case sensitive (1) or case insensitive (0) */
CJSON_PUBLIC(cJSON_bool) cJSON_Compare(const cJSON * const a, const cJSON * const b, const cJSON_bool case_sensitive);

/* Minify a strings, remove blank characters(such as ' ', '\t', '\r', '\n') from strings.
 * The input pointer json cannot point to a read-only address area, such as a string constant, 
 * but should point to a readable and writable address area. */
CJSON_PUBLIC(void) cJSON_Minify(char *json);

/* Helper functions for creating and adding items to an object at the same time.
 * They return the added item or NULL on failure. */
CJSON_PUBLIC(cJSON*) cJSON_AddNullToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON*) cJSON_AddTrueToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON*) cJSON_AddFalseToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON*) cJSON_AddBoolToObject(cJSON * const object, const char * const name, const cJSON_bool boolean);
CJSON_PUBLIC(cJSON*) cJSON_AddNumberToObject(cJSON * const object, const char * const name, const double number);
CJSON_PUBLIC(cJSON*) cJSON_AddStringToObject(cJSON * const object, const char * const name, const char * const string);
CJSON_PUBLIC(cJSON*) cJSON_AddRawToObject(cJSON * const object, const char * const name, const char * const raw);
CJSON_PUBLIC(cJSON*) cJSON_AddObjectToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON*) cJSON_AddArrayToObject(cJSON * const object, const char * const name);

/* When assigning an integer value, it needs to be propagated to valuedouble too. */
#define cJSON_SetIntValue(object, number) ((object) ? (object)->valueint = (object)->valuedouble = (number) : (number))
/* helper for the cJSON_SetNumberValue macro */
CJSON_PUBLIC(double) cJSON_SetNumberHelper(cJSON *object, double number);
#define cJSON_SetNumberValue(object, number) ((object != NULL) ? cJSON_SetNumberHelper(object, (double)number) : (number))
/* Change the valuestring of a cJSON_String object, only takes effect when type of object is cJSON_String */
CJSON_PUBLIC(char*) cJSON_SetValuestring(cJSON *object, const char *valuestring);

/* If the object is not a boolean type this does nothing and returns cJSON_Invalid else it returns the new type*/
#define cJSON_SetBoolValue(object, boolValue) ( \
    (object != NULL && ((object)->type & (cJSON_False|cJSON_True))) ? \
    (object)->type=((object)->type &(~(cJSON_False|cJSON_True)))|((boolValue)?cJSON_True:cJSON_False) : \
    cJSON_Invalid\
)

/* Macro for iterating over an array or object */
#define cJSON_ArrayForEach(element, array) for(element = (array != NULL) ? (array)->child : NULL; element != NULL; element = element->next)

/* malloc/free objects using the malloc/free functions that have been set with cJSON_InitHooks */
CJSON_PUBLIC(void *) cJSON_malloc(size_t size);
CJSON_PUBLIC(void) cJSON_free(void *object);

#ifdef __cplusplus
}
#endif

#endif

#define PORT 8080
#define BUFFER_SIZE 1024


#pragma comment(lib, "ws2_32.lib")

// 初始化 Winsock 库
int init_winsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed. Error Code : %d", WSAGetLastError());
        return 1;
    }
    return 0;
}

//创建和绑定服务器套接字
SOCKET create_server_socket() {
    SOCKET server_socket;
    struct sockaddr_in server_addr;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code : %d", WSAGetLastError());
        return INVALID_SOCKET;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code : %d", WSAGetLastError());
        return INVALID_SOCKET;
    }

    return server_socket;
}

// 处理客户端请求并返回响应
void handle_client_request(SOCKET client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_received;

    bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received > 0) {
        // 解析 HTTP 请求头（这里只是一个简单的示例，不做完整的请求解析）
        buffer[bytes_received] = '\0';
        printf("Request received: \n%s\n", buffer);

        // 创建一个简单的 HTTP 响应
        const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
                               "<html><body><h1>Welcome to My Server</h1></body></html>";

        send(client_socket, response, strlen(response), 0);
    }

    closesocket(client_socket);
}

//定义了一个名为visitor的结构体，用于存储游客的信息
typedef struct visitor {
    char id[20]; //身份证号
    char name[20]; //姓名
    char sex[2]; //性别
    int age; //年龄
    char remark[100]; //备注
    struct visitor *next; //指向下一个节点的指针
} visitor;

//定义链表结构体
typedef struct list {//定义了一个名为list的结构体，用于存储链表的头节点和长度
    visitor *head; //指向链表头节点的指针
    int size; //链表的长度
} list;

//定义全局变量
list *vis_list; //游客链表
char filename[] = "visitor.txt"; //保存游客信息的文件名
char password[] = "123456"; //修改信息的密码

//函数声明
void init_list(); //初始化链表
void input_info(); //录入游客信息
void display_info(); //显示游客信息
void save_info(); //保存游客信息
void delete_info(); //删除游客信息
void modify_info(); //修改游客信息
void search_info(); //查询游客信息
void free_list(); //释放链表
visitor *create_node(); //创建节点
void insert_node(visitor *node); //插入节点
void delete_node(char *id); //删除节点
visitor *find_node(char *id); //查找节点
void print_node(visitor *node); //打印节点
void print_menu(); //打印菜单
int check_password(); //检查密码
void modify_password(char *password);
int check_password_strength(char*password);//检查密码强度

void handle_http_request(SOCKET clientSocket) {
    // 创建一个JSON对象
    cJSON *json = cJSON_CreateObject();

    // 将JSON对象转换为字符串
    char *json_string = cJSON_Print(json);
    if (json_string == NULL) {
        // 处理错误
        cJSON_Delete(json);
        closesocket(clientSocket);
        return;
    }

    // 发送HTTP响应头
    const char *response_headers = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
    send(clientSocket, response_headers, strlen(response_headers), 0);

    // 发送JSON响应体
    send(clientSocket, json_string, strlen(json_string), 0);

    // 清理
    cJSON_free(json_string);
    cJSON_Delete(json);
    closesocket(clientSocket);
}

//主函数
int main() {

    char *json_string;
    char name[20],id[20],password[20];
    scanf(*json_string,"%s %s %s",name,id,password);
   check_password(password); //检查密码
   void modify_password(char *password);
int check_password_strength(char*password); 


    if (init_winsock() != 0) {
        return 1;
    }

    SOCKET server_socket = create_server_socket();
    if (server_socket == INVALID_SOCKET) {
        WSACleanup();
        return 1;
    }

    // 开始监听端口
    if (listen(server_socket, 5) == SOCKET_ERROR) {
        printf("Listen failed. Error Code : %d", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server is running on port %d...\n", PORT);

    while (1) {
        SOCKET client_socket = accept(server_socket, NULL, NULL);
        if (client_socket != INVALID_SOCKET) {
            handle_client_request(client_socket);
        }
    }

    // 关闭服务器套接字
    closesocket(server_socket);
    WSACleanup();
    return 0;

    //accept
    SOCKADDR_IN clientAddr;
    int  len1 = sizeof(clientAddr);
    SOCKET clientSocket = accept(server_socket,(SOCKADDR*)&clientAddr,&len1);
    char recvBuff[128];
    while(1){
        memset(recvBuff,0,sizeof(recvBuff));
        //recv();
        recv(clientSocket,recvBuff,sizeof(recvBuff)-1,0);
        printf("receive message: %s\n",recvBuff);
        char sendBuff[128];
        memset(sendBuff,0,size0f(sendBuff));
        printf("Please input:\n");
        scanf("%s",sendBuff);
        send(clientSocket,sendBuff,strlen(sendBuff),0);
    }

    closesocket(clientSocket);
    closesocket(server_socket);
    return 0;

    if(check_password())
    {   
        check_password_strength(password);
        char choice; //用户选择的功能项
        init_list(); //初始化链表
        while (1) {
            print_menu(); //打印菜单
            printf("请选择系统功能项：\n");
            scanf("%c", &choice); //输入选择
            getchar(); //清除缓冲区的换行符
            switch (choice) {
                case 'a': //录入信息
                    input_info();
                    break;
                case 'b': //显示信息
                    display_info();
                    break;
                case 'c': //保存信息
                    save_info();
                    break;
                case 'd': //删除信息
                    delete_info();
                    break;
                case 'e': //修改信息
                    modify_info();
                    break;
                case 'f': //查询信息
                    search_info();
                    break;
                case 'g': //退出系统
                    free_list(); //释放链表
                    printf("感谢您使用本系统，再见！\n");
                    return 0; //结束程序
                case 'h': //修改密码
                    modify_password(password);
                    break;
                default: //无效选择
                    printf("\033[91m无效的选择，请重新输入。\033[0m\n");
                    break;
            }
            break;
        }
    }
}


//创建节点
visitor *create_node() {
    visitor *node = (visitor *)malloc(sizeof(visitor)); //分配内存空间
    if (node == NULL) { //分配失败
        printf("内存分配失败，无法创建节点。\n");
        exit(1); //退出程序
    }
    node->next = NULL; //指针为空
    return node; //返回节点
}

//初始化链表
void init_list() {
    vis_list = (list *)malloc(sizeof(list)); //分配内存空间
    if (vis_list == NULL) { //分配失败
        printf("内存分配失败，无法初始化链表。\n");
        exit(1); //退出程序
    }
    vis_list->head = NULL; //头节点为空
    vis_list->size = 0; //链表长度为0
    FILE *fp = fopen(filename, "r"); //打开文件
    if (fp == NULL) { //打开失败
        printf("无法打开文件%s，可能是第一次使用本系统。\n", filename);
        return; //返回
    }
    visitor *node; //临时节点
    while (1) {
        node = create_node(); //创建节点
        if (fscanf(fp, "%s %s %s %d %s", node->id, node->name, node->sex, &node->age, node->remark) == EOF) { //读取文件到节点
            free(node); //释放节点
            break; //跳出循环
        }
        insert_node(node); //插入节点
    }
    fclose(fp); //关闭文件
    printf("已从文件%s中读取游客信息。\n", filename);
}

//插入节点
void insert_node(visitor *node) {
    if (vis_list->head == NULL) { //链表为空
        vis_list->head = node; //头节点为新节点
    } else { //链表不为空
        visitor *p = vis_list->head; //从头节点开始
        while (p->next != NULL) { //遍历链表
            p = p->next; //指向下一个节点
        }
        p->next = node; //尾节点的指针指向新节点
    }
    vis_list->size++; //链表长度加一
}


//录入学生信息
void input_info() {
    printf("请输入游客的基本信息，以空格分隔，以回车结束。\n");
    printf("格式：身份证号 姓名 性别 年龄 备注\n");
    visitor *node = create_node(); //创建节点
    scanf("%s %s %s %d %s", node->id, node->name, node->sex, &node->age, node->remark); //输入信息到节点
    getchar(); //清除缓冲区的换行符
    if (find_node(node->id) != NULL) { //查找是否已存在相同学号的节点
        printf("该身份证号已存在，无法录入。\n");
        free(node); //释放节点
        return; //返回
    }
    insert_node(node); //插入节点
    printf("已成功录入游客信息。\n");
}

//显示学生信息
void display_info() {
    if (vis_list->size == 0) { //链表为空
        printf("没有游客信息可以显示。\n");
        return; //返回
    }
    printf("以下是所有游客的基本信息：\n");
    visitor *node = vis_list->head; //从头节点开始
    while (node != NULL) { //遍历链表
        print_node(node); //打印节点
        node = node->next; //指向下一个节点
    }
}

//保存学生信息
void save_info() {
    if (vis_list->size == 0) { //链表为空
        printf("没有游客信息可以保存。\n");
        return; //返回
    }
    FILE *fp = fopen(filename, "w+"); //打开文件
    if (fp == NULL) { //打开失败
        printf("无法打开文件%s，无法保存游客信息。\n", filename);
        return; //返回
    }
    visitor *node = vis_list->head; //从头节点开始
    while (node != NULL) { //遍历链表
        fprintf(fp, "%s %s %s %d %s\n", node->id, node->name, node->sex, node->age, node->remark); //写入文件
        node = node->next; //指向下一个节点
    }
    fclose(fp); //关闭文件
    printf("已成功保存游客信息到文件%s。\n", filename);
}

//删除游客信息
void delete_info() {
    if (vis_list->size == 0) { //链表为空
        printf("没有游客信息可以删除。\n");
        return; //返回
    }
    char id[20]; //要删除的身份证号
    printf("请输入要删除的游客的身份证号：\n");
    scanf("%s", id); //输入身份证号
    getchar(); //清除缓冲区的换行符
    visitor *node = find_node(id); //查找节点
    if (node == NULL) { //未找到
        printf("没有找到该身份证号的游客，无法删除。\n");
        return; //返回
    }
    delete_node(id); //删除节点
    printf("已成功删除游客信息。\n");
}

//修改游客信息
void modify_info() {
    if (vis_list->size == 0) { //链表为空
        printf("没有游客信息可以修改。\n");
        return; //返回
    }
    check_password(); //检查密码
    char id[20]; //要修改的身份证号
    printf("请输入要修改的游客的身份证号：\n");
    scanf("%s", id); //输入学号
    getchar(); //清除缓冲区的换行符
    visitor *node = find_node(id); //查找节点
    if (node == NULL) { //未找到
        printf("没有找到该身份证号的游客，无法修改。\n");
        return; //返回
    }
    printf("请输入游客的新信息，以空格分隔，以回车结束。\n");
    printf("格式：身份证号 姓名 性别 年龄 备注\n");
    scanf("%s %s %s %d %s", node->id, node->name, node->sex, &node->age, node->remark); //输入新信息到节点
    getchar(); //清除缓冲区的换行符
    printf("已成功修改游客信息。\n");
}

//查询游客信息
void search_info() {
    if (vis_list->size == 0) { //链表为空
        printf("没有游客信息可以查询。\n");
        return; //返回
    }
    char choice; //用户选择的查询方式
    printf("请选择查询方式：\n");
    printf("(1)按身份证号查询\n");
    printf("(2)按姓名查询\n");
    printf("(3)按性别查询\n");
    printf("(4)按年龄查询\n");
    scanf("%c", &choice); 
    getchar(); //清除缓冲区的换行符
    char key[20]; //查询关键字
    int count = 0; //查询结果的数量
    switch (choice) {
        case '1': //按身份证号查询
            printf("请输入要查询的学号：\n");
            scanf("%s", key); //输入学号
            getchar(); //清除缓冲区的换行符
            visitor *node = find_node(key); //查找节点
            if (node == NULL) { //未找到
                printf("没有找到该身份证的游客。\n");
                return; //返回
            }
            printf("以下是查询结果：\n");
            print_node(node); //打印节点
            break;
        case '2': //按姓名查询
            printf("请输入要查询的姓名：\n");
            scanf("%s", key); //输入姓名
            getchar(); //清除缓冲区的换行符
            visitor *p = vis_list->head; //从头节点开始
            printf("以下是查询结果：\n");
            while (p != NULL) { //遍历链表
                if (strcmp(p->name, key) == 0) { //匹配姓名
                    print_node(p); //打印节点
                    count++; //计数加一
                }
                p = p->next; //指向下一个节点
            }
            if (count == 0) { //没有匹配结果
                printf("没有找到该姓名的游客。\n");
            }
            break;
        case '3': //按性别查询
            printf("请输入要查询的性别：\n");
            scanf("%s", key); //输入性别
            getchar(); //清除缓冲区的换行符
            visitor *q = vis_list->head; //从头节点开始
            printf("以下是查询结果：\n");
            while (q != NULL) { //遍历链表
                if (strcmp(q->sex, key) == 0) { //匹配性别
                    print_node(q); //打印节点
                    count++; //计数加一
                }
                q = q->next; //指向下一个节点
            }
            if (count == 0) { //没有匹配结果
                printf("没有找到该性别的游客。\n");
            }
            break;
        case '4': //按年龄查询
            printf("请输入要查询的年龄：\n");
            scanf("%s", key); //输入年龄
            getchar(); //清除缓冲区的换行符
            int age = atoi(key); //转换为整数
            visitor *r = vis_list->head; //从头节点开始
            printf("以下是查询结果：\n");
            while (r != NULL) { //遍历链表
                if (r->age == age) { //匹配年龄
                    print_node(r); //打印节点
                    count++; //计数加一
                }
                r = r->next; //指向下一个节点
            }
            if (count == 0) { //没有匹配结果
                printf("没有找到该年龄的游客。\n");
            }
            break;
        default: //无效选择
            printf("无效的选择，请重新输入。\n");
    }
}


//释放链表
void free_list() {
    visitor *node = vis_list->head; //从头节点开始
    visitor *temp; //临时节点
    while (node != NULL) { //遍历链表
        temp = node; //保存当前节点
        node = node->next; //指向下一个节点
        free(temp); //释放当前节点
    }
    free(vis_list); //释放链表
}


//删除节点
void delete_node(char *id) {
    if (vis_list->head == NULL) { //链表为空
        printf("链表为空，无法删除节点。\n");
        return; //返回
    }
    visitor *p = vis_list->head; //从头节点开始
    visitor *prev = NULL; //前一个节点
    while (p != NULL) { //遍历链表
        if (strcmp(p->id, id) == 0) { //匹配学号
            if (prev == NULL) { //头节点
                vis_list->head = p->next; //头节点指向下一个节点
            } else { //非头节点
                prev->next = p->next; //前一个节点的指针指向下一个节点
            }
            free(p); //释放节点
            vis_list->size--; //链表长度减一
            return; //返回
        }
        prev = p; //保存当前节点
        p = p->next; //指向下一个节点
    }
}

//查找节点
visitor *find_node(char *id) {
    visitor *node = vis_list->head; //从头节点开始
    while (node != NULL) { //遍历链表
        if (strcmp(node->id, id) == 0) { //匹配学号
            return node; //返回节点
        }
        node = node->next; //指向下一个节点
    }
    return NULL; //未找到
}

//打印节点
void print_node(visitor *node) {
    printf("身份证号：%s\n", node->id);
    printf("姓名：%s\n", node->name);
    printf("性别：%s\n", node->sex);
    printf("年龄：%d\n", node->age);
    printf("备注：%s\n", node->remark);
    printf("**********\n");
}

//打印菜单
void print_menu() {
    printf("请选择系统功能项：\n");
    printf("a   游客基本信息录入\n");
    printf("b   游客基本信息显示\n");
    printf("c   游客基本信息保存\n");
    printf("d   游客基本信息删除\n");
    printf("e   游客基本信息修改（要求先输入密码）\n");
    printf("f   游客基本信息查询\n");
    printf("g   退出系统\n");
    printf("h   修改密码(需重新登陆系统)\n");
}

//检查密码
int check_password() {
    char input[20]; //用户输入的密码
    int count = 0; //尝试次数
    while (1) {
        printf("请输入密码：\n");
        scanf("%s", input); //输入密码
        getchar(); //清除缓冲区的换行符
        if (strcmp(input, password) == 0) { //密码正确
            printf("密码正确，欢迎进入系统。\n");
            return 1; //跳出循环
        } else { //密码错误
            printf("密码错误，请重新输入。\n");
            count++; //计数加一
            if (count == 3) { //尝试次数达到3次
                printf("您已经连续输入错误3次，无法登入系统。\n");
                exit(EXIT_FAILURE); //退出程序
            }
        }
    }
}

int check_password_strength(char*password) {//对密码强度的检测
    int length = strlen(password);
    int has_lower = 0;
    int has_upper = 0;
    int has_digit = 0;
    int has_special = 0;

    for (int i = 0; i < length; i++) {
        if(islower(password[i])) {
            has_lower = 1;
        }
        if(isupper(password[1])) {
            has_upper = 1;
        }
        if (isdigit(password[i])) {
            has_digit = 1;
        }
        if (!isalnum(password[i])) {
            has_special = 1;
        }
    }
    if (length < 8 || !(has_lower && has_upper && has_digit && has_special)) {
        return 0;//弱
    } else if (length < 12 || !(has_lower && has_upper && has_digit && has_special)) {
        return 1;//中等
    } else {
        return 2;//强
    }
}


void modify_password(char *password)
{
    int count = 0;
    char verify_password[20];
    char temp[20];
    printf("请输入原先的密码: \n");
    scanf("%s",verify_password);
    getchar(); // 清除缓冲区的换行符
    while(1)
    {
        if (strcmp(verify_password, password) == 0)
        { // 密码正确
            printf("请输入新的密码：\n");
            scanf("%s",temp);
            strcpy(password,temp);
            break; // 跳出循环
        } 
        else{ // 密码错误
            printf("密码错误，请重新输入。\n");
            count++; 
            // 计数加一
            if (count == 3)
            { // 尝试次数达到3次
                printf("您已连续输入错误3次，无法修改密码。\n");
                return; // 退出函数
            }
            printf("请输入原先的密码: \n");
            scanf("%s",verify_password);
            getchar(); // 清除缓冲区的换行符
        }
    }
    check_password();
}

