#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<ctype.h>
#include<stdbool.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#define LENGTH 13
#define LEN 50
#define MAX_NAME_LEN 50
#define MAX_USERS 1000
#define MAX_RESERVER 800

//定义了一个名为visitor的结构体，用于存储游客的信息
typedef struct visitor {
    char id[20]; //身份证号
    char name[20]; //姓名
    char sex[2]; //性别
    int age; //年龄
    char password[50];
    char remark[100]; //备注
    struct visitor *next; //指向下一个节点的指针
} visitor;
 
//定义了一个名为Appointment的结构体，由于存储游客预约信息
typedef struct node {
    char userName[LEN];
    int startHour;
    int endHour;
    int month;
    int day;
    int year;
    long long int phonenumber;
    char campus_name[LENGTH];
    struct node* next;
} Appointment;

typedef struct VisitorAppointment {
    visitor visitorInfo;
    Appointment appointmentInfo;
    struct VisitorAppointment* next;
} VisitorAppointment;

//定义链表结构体
typedef struct list {//定义了一个名为list的结构体，用于存储链表的头节点和长度
    VisitorAppointment *head; //指向链表头节点的指针
    int size; //链表的长度
} list;

visitor user[MAX_USERS];
Appointment* head = NULL;
Appointment res[MAX_RESERVER];
char campus_name[2][13] = { "QING SHUI HE", "SHA HE" };
int userNumber = 0;
int reserver = 0;
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
void free_list(); //
VisitorAppointment *create_node(); //创建节点
void insert_node(VisitorAppointment *node); //插入节点
void delete_node(char *id); //删除节点
VisitorAppointment *find_node(char *id); //查找节点
void print_node(VisitorAppointment *node); //打印节点
void print_menu(); //打印菜单
int check_password(); //检查密码
void modify_password(char *password);
const char *check_password_strength(const char *password);//检查密码强度
void  fun(char* p);
bool is_leapYear(int year);//判断闰年
void insert_appointment();//预约
void check_appointment();// 检查预约是否成功
void delete_user();//删除用户

//主函数
int main() {
    char password[20];
    printf("Please enter your password: ");
    scanf("%s", password);
    const char* strength = check_password_strength(password);
printf("Password strength: %s\n", strength);
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
                    printf("无效的选择，请重新输入。\n");

                     // 设置定时器，每60秒调用一次timer_handler
        struct itimerval timer;
        timer.it_value.tv_sec = 60; // 定时器启动时间（立即）
        timer.it_value.tv_usec = 0;
        timer.it_interval.tv_sec = 60; // 定时器间隔时间
        timer.it_interval.tv_usec = 0;

              signal(SIGALRM, timer_handler);
              setitimer(ITIMER_REAL, &timer, NULL);

            }
        }
    }
    char password[20];

}

//创建节点
VisitorAppointment *create_node() {
    VisitorAppointment *node = (VisitorAppointment *)malloc(sizeof(VisitorAppointment)); //分配内存空间
    if (node == NULL) { //分配失败
        printf("内存分配失败，无法创建节点。\n");
        exit(1); //退出程序
    }
    node->next = NULL; //指针为空
    return node; //返回节点
}

void delete_expired_appointments() {
    time_t now;
    struct tm *now_tm;
    time(&now);
    now_tm = localtime(&now);

    int i = 0;
    while (i < reserver) {
        if (res[i].year < now_tm->tm_year + 1900 ||
            (res[i].year == now_tm->tm_year + 1900 && res[i].month < now_tm->tm_mon) ||
            (res[i].year == now_tm->tm_year + 1900 && res[i].month == now_tm->tm_mon && res[i].day < now_tm->tm_mday)) {
            // 找到过时的预约，从数组中移除
            for (int j = i; j < reserver - 1; j++) {
                res[j] = res[j + 1];
            }
            reserver--;
            i--;
        }
        i++;
    }
}
void timer_handler(int sig) {
    delete_expired_appointments();
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
    VisitorAppointment *node; //临时节点
    while (1) {
        node = create_node(); //创建节点
        if (fscanf(fp, "%s %s %s %d %s %s %s %d %d %d %d %d %lld %s\n", node->visitorInfo.id, node->visitorInfo.name, node->visitorInfo.sex, &node->visitorInfo.age, node->visitorInfo.remark, node->visitorInfo.password,node->appointmentInfo.userName, node->appointmentInfo.startHour, node->appointmentInfo.endHour, node->appointmentInfo.month, node->appointmentInfo.day, node->appointmentInfo.year, node->appointmentInfo.phonenumber, node->appointmentInfo.campus_name) == EOF) { //读取文件到节点
            free(node); //释放节点
            break; //跳出循环
        }
        insert_node(node); //插入节点
    }
    fclose(fp); //关闭文件
    printf("已从文件%s中读取游客信息。\n", filename);
}

//插入节点
void insert_node(VisitorAppointment *node) {
    if (vis_list->head == NULL) { //链表为空
        vis_list->head = node; //头节点为新节点
    } else { //链表不为空
        VisitorAppointment *p = vis_list->head; //从头节点开始
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
    printf("格式：身份证号 姓名 性别 年龄 备注 密码\n");
    VisitorAppointment *node = create_node(); //创建节点
    scanf("%s %s %s %d %s %s %s %d %d %d %d %d %lld %s", node->visitorInfo.id, node->visitorInfo.name, node->visitorInfo.sex, &node->visitorInfo.age, node->visitorInfo.remark, node->visitorInfo.password, node->appointmentInfo.userName, node->appointmentInfo.startHour, node->appointmentInfo.endHour, node->appointmentInfo.month, node->appointmentInfo.day, node->appointmentInfo.year, node->appointmentInfo.phonenumber, node->appointmentInfo.campus_name); //输入信息到节点
    getchar(); //清除缓冲区的换行符
    if (find_node(node->visitorInfo.id) != NULL) { //查找是否已存在相同学号的节点
        printf("该身份证号已存在，无法录入。\n");
        free(node); //释放节点
        return; //返回
    }
    insert_node(node); //插入节点
    userNumber++;
    printf("已成功录入游客信息。\n");
}

//显示学生信息
void display_info() {
    if (vis_list->size == 0) { //链表为空
        printf("没有游客信息可以显示。\n");
        return; //返回
    }
    printf("以下是所有游客的基本信息：\n");
    VisitorAppointment *node = vis_list->head; //从头节点开始
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
    VisitorAppointment *node = vis_list->head; //从头节点开始
    while (node != NULL) { //遍历链表
        fprintf(fp, "%s %s %s %d %s %s %s %d %d %d %d %d %lld %s", node->visitorInfo.id, node->visitorInfo.name, node->visitorInfo.sex, &node->visitorInfo.age, node->visitorInfo.remark, node->visitorInfo.password, node->appointmentInfo.userName, node->appointmentInfo.startHour, node->appointmentInfo.endHour, node->appointmentInfo.month, node->appointmentInfo.day, node->appointmentInfo.year, node->appointmentInfo.phonenumber, node->appointmentInfo.campus_name); //写入文件
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
    VisitorAppointment *node = find_node(id); //查找节点
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
    VisitorAppointment *node = find_node(id); //查找节点
    if (node == NULL) { //未找到
        printf("没有找到该身份证号的游客，无法修改。\n");
        return; //返回
    }
    printf("请输入游客的新信息，以空格分隔，以回车结束。\n");
    printf("格式：身份证号 姓名 性别 年龄 备注 密码\n");
    scanf("%s %s %s %d %s %s",node->visitorInfo.id, node->visitorInfo.name, node->visitorInfo.sex, &node->visitorInfo.age, node->visitorInfo.remark, node->visitorInfo.password); //输入新信息到节点
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
            VisitorAppointment *node = find_node(key); //查找节点
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
            VisitorAppointment *p = vis_list->head; //从头节点开始
            printf("以下是查询结果：\n");
            while (p != NULL) { //遍历链表
                if (strcmp(p->visitorInfo.name, key) == 0) { //匹配姓名
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
            VisitorAppointment *q = vis_list->head; //从头节点开始
            printf("以下是查询结果：\n");
            while (q != NULL) { //遍历链表
                if (strcmp(p->visitorInfo.sex, key) == 0) { //匹配性别
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
            VisitorAppointment *r = vis_list->head; //从头节点开始
            printf("以下是查询结果：\n");
            while (r != NULL) { //遍历链表
                if (p->visitorInfo.age == age) { //匹配年龄
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
    VisitorAppointment *node = vis_list->head; //从头节点开始
    VisitorAppointment *temp; //临时节点
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
    VisitorAppointment *p = vis_list->head; //从头节点开始
    VisitorAppointment *prev = NULL; //前一个节点
    while (p != NULL) { //遍历链表
        if (strcmp(p->visitorInfo.id, id) == 0) { //匹配学号
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
VisitorAppointment *find_node(char *id) {
    VisitorAppointment *node = vis_list->head; //从头节点开始
    while (node != NULL) { //遍历链表
        if (strcmp(node->visitorInfo.id, id) == 0) { //匹配学号
            return node; //返回节点
        }
        node = node->next; //指向下一个节点
    }
    return NULL; //未找到
}

//打印节点
void print_node(VisitorAppointment *node) {
    printf("身份证号：%s\n", node->visitorInfo.id);
    printf("姓名：%s\n", node->visitorInfo.name);
    printf("性别：%s\n", node->visitorInfo.sex);
    printf("年龄：%d\n", node->visitorInfo.age);
    printf("备注：%s\n", node->visitorInfo.remark);
    printf("密码：%s\n", node->visitorInfo.password);
    printf("用户名：%s\n", node->appointmentInfo.userName);
    printf("起始时间：%d\n", node->appointmentInfo.startHour);
    printf("截止时间：%d\n", node->appointmentInfo.endHour);
    printf("月份：%d\n", node->appointmentInfo.month);
    printf("天数：%d\n", node->appointmentInfo.day);
    printf("年份：%d\n", node->appointmentInfo.year);
    printf("电话号：%d\n", node->appointmentInfo.phonenumber);
    printf("校区：%s\n", node->appointmentInfo.campus_name);
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
                exit(1); //退出程序
            }
        }
    }
}

const char *check_password_strength(const char *password) {
    int length = strlen(password);
    int has_lower = 0, has_upper = 0, has_digit = 0, has_special = 0;

    for (int i = 0; i < length; i++) {
        if (islower(password[i])) has_lower = 1;
        if (isupper(password[i])) has_upper = 1;
        if (isdigit(password[i])) has_digit = 1;
        if (!isalnum(password[i])) has_special = 1;
    }

    if (length < 8 || !(has_lower && has_upper && has_digit && has_special)) {
        return "Weak";
    } else if (length < 12 || !(has_lower && has_upper && has_digit && has_special)) {
        return "Medium";
    } else {
        return "Strong";
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


void fun(char* p) {
    int k = 0;
    while (*p) {
        if (k == 0 && *p != ' ') {
            *p = toupper(*p);
            k = 1;
        }
        else if (*p != ' ') {
            k = 1;
        }
        else {
            k = 0;
        }
        p++;
    }
}

bool is_leapYear(int year) {
    if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))
        return true;
    else
        return false;
}
void insert_appointment() { // 预约
    VisitorAppointment * p = (VisitorAppointment*)malloc(sizeof(VisitorAppointment));
    if (p == NULL) {
        printf("The booking is invalid!");
        return;
    }
    if (reserver >= MAX_RESERVER) {
        printf("Reserver limit reached.\n");
        return;
    }
    char Campusname[LENGTH];
    char ID[LEN];
    char username[LEN];
    int i, j, IDfound = 0, Campusfound = 0;
    printf("Enter the ID of user: ");
    scanf("%s", ID);
    getchar();
    printf("\n");
    for (i = 0; i < userNumber; i++) {
        if (strcmp(user[i].id, ID) == 0) {
            printf("Find the user!\n");
            IDfound = 1;
            break;
        }
    }
    printf("Please enter the name of user:");
    fgets(username, LEN, stdin);
    username[strcspn(username, "\n")] = 0;
    fun(username);
    for (i = 0;i < reserver;i++) {
        if (strcmp(res[i].userName, username) == 0) {
            printf("该用户已预约\n");
            return;
        }
    }
    printf("该用户第一次预约\n");
    if (!IDfound) {
        printf("Not find such a user!\n");
        free(p);
        return;
    }
    printf("Enter the name of campus: ");
    fgets(Campusname, LEN, stdin);
    Campusname[strcspn(Campusname, "\n")] = 0; // 确保字符串以空字符结尾
    printf("\n");
    for (j = 0; j < 2; j++) {
        if (strcmp(campus_name[j], Campusname) == 0) {
            printf("Find the campus!\n");
            strcpy(p->appointmentInfo.campus_name, campus_name[j]);
            Campusfound = 1;
            break;
        }
    }
    strcpy(p->appointmentInfo.campus_name, "");
    if (!Campusfound) {
        printf("Not find the campus!\n");
        free(p);
        return;
    }
    printf("Booking the campus successfully!\n");
    int a, b, c, d, e;
    long long int f;
    printf("Start to reserve!\n");
    printf("预约起止时间（例如 9 17）：");
    scanf("%d %d", &a, &b);
    printf("预约日期（例如 2 8 2024）：");
    scanf("%d %d %d", &c, &d, &e);
    printf("预约手机号: ");
    scanf("%lld", &f);
    while (getchar() != '\n'); // 清除缓冲区
    printf("Enter your name: ");
    fgets(p->appointmentInfo.userName, LEN, stdin);
    p->appointmentInfo.userName[strcspn(p->appointmentInfo.userName, "\n")] = 0;
    fun(p->appointmentInfo.userName);
    p->appointmentInfo.startHour = a;
    p->appointmentInfo.endHour = b;
    p->appointmentInfo.month = c;
    p->appointmentInfo.day = d;
    p->appointmentInfo.year = e;
    p->appointmentInfo.phonenumber = f;
    if (a < 8 || b > 18 || c < 1 || c > 12 || (e != 2024 && e != 2025)) {
        printf("The booking is unsuccessful!\n");
        free(p);
        return;
    }
    if (c == 1 || c == 3 || c == 5 || c == 7 || c == 8 || c == 10 || c == 12) {
        if (d < 1 || d > 31) {
            printf("The booking is unsuccessful!\n");
            free(p);
            return;
        }
    }
    else if (c == 4 || c == 6 || c == 9 || c == 11) {
        if (d < 1 || d > 30) {
            printf("The booking is unsuccessful!\n");
            free(p);
            return;
        }
    }
    else {
        if (is_leapYear(e)) {
            if (d < 1 || d > 29) {
                printf("The booking is unsuccessful!\n");
                free(p);
                return;
            }
        }
        else {
            if (d < 1 || d > 28) {
                printf("The booking is unsuccessful!\n");
                free(p);
                return;
            }
        }
    }
    printf("The booking is successful!\n");
    p->next = head;
    head = p;
    res[reserver++] = p->appointmentInfo;
}

void check_appointment() { // 检查预约是否成功
    char name[MAX_NAME_LEN];
    int i, j, found = 0;
    printf("Please enter the username to check: ");
    fgets(name, MAX_NAME_LEN, stdin);
    name[strcspn(name, "\n")] = 0;
    fun(name);
    for (i = 0; i < userNumber; i++) {
        if (strcmp(user[i].name, name) == 0) {
            printf("Username: %s\tID: %s\n", user[i].name, user[i].id);
            for (j = 0; j < reserver; j++) {
                if (strcmp(res[j].userName, name) == 0) {
                    printf("Reservation found: starthour: %d endhour: %d\nDate: day: %d, month: %d, year: %d\nphonenumber: %lld\n",
                        res[j].startHour, res[j].endHour, res[j].day, res[j].month, res[j].year, res[j].phonenumber);
                    printf("Reservation campus: %s\n", res[j].campus_name);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("No reservation found for this user.\n");
            }
            return;
        }
    }
    printf("User not found.\n");
}

void delete_user() {         //删除用户 
    char ID[LEN];
    long long int phonenumber;
    printf("Enter your ID:");
    fgets(ID, LEN, stdin);
    ID[strcspn(ID, "\n")] = 0;
    printf("Enter your phonenumber:");
    scanf("%lld", &phonenumber);
    while (getchar() != '\n');
    int i, j;
    for (i = 0;i < userNumber;i++) {
        if (strcmp(user[i].id, ID) == 0) {
            for (j = i;j < userNumber - 1;j++) {
                user[j] = user[j + 1];
            }
            userNumber--;
            break;
        }
    }
    for (i = 0;i < reserver;i++) {
        if (res[i].phonenumber == phonenumber) {
            for (j = i;j < reserver - 1;j++) {
                res[j] = res[j + 1];
            }
            reserver--;
            break;
        }
    }
}

