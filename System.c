#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define COBJMACROS
#include <stdio.h>
#include <MsXml6.h>
#include <assert.h>
#include <time.h>
#include <Windows.h>
#include <sys/stat.h>
#define MAX_LEN 100

int check_user(const char *username, const char *password) {
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        return 0;  // 文件打开失败
    }

    char stored_username[MAX_LEN], stored_password[MAX_LEN];
    while (fscanf(file, "%s %s", stored_username, stored_password) != EOF) {
        if (strcmp(stored_username, username) == 0 && strcmp(stored_password, password) == 0) {
            fclose(file);
            return 1;  // 用户名和密码匹配
        }
    }

    fclose(file);
    return 0;  // 用户名或密码错误
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

//主函数
int main() {

     char *data;
    char username[MAX_LEN], password[MAX_LEN];
    
    // 获取POST请求的表单数据
    data = getenv("QUERY_STRING");
    if (data != NULL) {
        sscanf(data, "username=%s&password=%s", username, password);
        
        // URL 解码
        for (int i = 0; username[i]; i++) if (username[i] == '+') username[i] = ' ';
        for (int i = 0; password[i]; i++) if (password[i] == '+') password[i] = ' ';
        
        // 验证用户
        if (check_user(username, password)) {
            printf("Content-Type: text/html\n\n");
            printf("<h1>登录成功！</h1>");
        } else {
            printf("Content-Type: text/html\n\n");
            printf("<h1>用户名或密码错误</h1>");
        }
    } else {
        printf("Content-Type: text/html\n\n");
        printf("<h1>表单提交失败</h1>");
    }
    
    return 0;

     // 检查文件权限
    struct stat file_stat;
    if (stat("/usr/lib/cgi-bin/login.cgi", &file_stat) == 0) {
        printf("File permission: %o\n", file_stat.st_mode);
    } else {
        perror("Error getting file status");
    }

    // 设置文件权限
    if (chmod("/usr/lib/cgi-bin/login.cgi", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == 0) {
        printf("File permission changed successfully.\n");
    } else {
        perror("Error changing file permission");
    }

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
    scanf("%c", &choice); //输入选择
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

