#include<stdio.h>
#include<string.h>
#include<stdlib.h> 
#include<stdbool.h>
#include<ctype.h>

#define LEN 50
#define NUMBER 12
#define MAX_NAME_LEN 50
#define MAX_USERS 1000
#define MAX_RESERVER 800

typedef struct node {
    char userName[LEN];
    int startHour;
    int endHour;
    int month;
    int day;
    int year; 
    long long int phonenumber;
    char (*campus_name)[13];
    int booked;    
    struct node *next;
} Appointment;

typedef struct {
    char user_name[LEN];
    long int user_id;
} User;

User user[MAX_USERS];
Appointment *head = NULL;
Appointment res[MAX_RESERVER];
char campus_name[2][13] = {"QING SHUI HE", "SHE HE"};
res.campus_name=campus_name;
int userNumber = 0;
int reserver = 0;
const int capacity=800;

void fun(char *p) {
    int k = 0;
    while (*p) {
        if (k == 0 && *p != ' ') {
            *p = toupper(*p);
            k = 1;
        } else if (*p != ' ') {
            k = 1;
        } else {
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

void registerUser() { // 注册用户
    if (userNumber >= MAX_USERS) {
        printf("Users limit reached.\n");
        return;
    }
    printf("Enter your name: ");
    fgets(user[userNumber].user_name, LEN, stdin);
    user[userNumber].user_name[strcspn(user[userNumber].user_name, "\n")] = 0;
    fun(user[userNumber].user_name);
    printf("Enter your ID: ");
    scanf("%ld", &user[userNumber].user_id);
    while (getchar() != '\n'); // 清除缓冲区
    printf("User registered successfully.\n");
    userNumber++;
}

void insert_appointment() { // 预约
	Appointment *p = (Appointment *)malloc(sizeof(Appointment));
    if (p == NULL) {
        printf("The booking is invalid!");
        return;
    }
	char Campusname[LEN];
	long int ID;
	int i,j,IDfound=0,Campusfound=0;
	printf("Enter the ID of user:");
	scanf("%ld",&ID);
	printf("\n");
	for(i=0;i<userNumber;i++){
	if(user->user_id==ID){
	  printf("Find the user!\n");
	  IDfound=1;
	  break;
	}
}if(!IDfound){
	printf("Not find such a user!\n");
	free(p);
	return;
} 
    printf("Enter the name of campus:");
    scanf("%s",Campusname);
    printf("\n");
   	for(j=0;j<=1;j++){
   	if(strcmp(p->campus_name[j],Campusname)==0){
   	printf("Find the campus!\n");
   	strcpy(p->campus_name[j],Campusname);
   	Campusfound=1;
   	break;
	   }
   }
if(!Campusfound){
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
    fgets(p->userName, LEN, stdin);
    p->userName[strcspn(p->userName, "\n")] = 0;
    fun(p->userName);
    p->startHour = a;
    p->endHour = b;
    p->month = c;
    p->day = d;
    p->year = e;
    p->phonenumber = f;
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
    } else if (c == 4 || c == 6 || c == 9 || c == 11) {
        if (d < 1 || d > 30) {
            printf("The booking is unsuccessful!\n");
            free(p);
            return;
        }
    } else {
        if (is_leapYear(e)) {
            if (d < 1 || d > 29) {
                printf("The booking is unsuccessful!\n");
                free(p);
                return;
            }
        } else {
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
    res[reserver++] = *p;
}

void check_appointment() { // 检查预约是否成功
    char name[MAX_NAME_LEN];
    int i,j;
    printf("Please enter the username to check: ");
    fgets(name, MAX_NAME_LEN, stdin);
    name[strcspn(name, "\n")] = 0;
    fun(name);
    for (i = 0; i < userNumber; i++) {
        if (strcmp(user[i].user_name, name) == 0) {
            printf("Username: %s\tID: %ld\n", user[i].user_name, user[i].user_id);
            printf("预约状态:%s",res[i].campus_name? "已预约":"未预约");
            printf("Reservation found: starthour: %d endhour: %d\nDate: day: %d, month: %d, year: %d\nphonenumber: %lld\n",
                    res[i].startHour, res[i].endHour, res[i].day, res[i].month, res[i].year, res[i].phonenumber);
        for(j=0;j<=1;j++)
		    printf("Reservation campus: %s",res[i].campus_name[j]); 
            }
            printf("No reservation found for this user.\n");
            return;
    }
    printf("User not found.\n");
}

bool arraysEqual(long long int a1, long long int a2) { // judge number are equal
    return a1 == a2;
}

void cancel_appointment() { // 取消预约
    printf("请输入手机号：");
    long long int phone_number;
    scanf("%lld", &phone_number);
    while (getchar() != '\n'); // 清除缓冲区
    Appointment *current = head, *prev = NULL;
    while (current != NULL) {
        if (arraysEqual(current->phonenumber, phone_number)) {
            if (prev == NULL) {
                head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            printf("预约取消成功!\n");
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("未找到该手机号的预约。\n");
}

int main() {
	int choice;
	scanf("%d",&choice);
	do{
		switch(choice){
    case 1:
    registerUser();break;
    case 2:
    insert_appointment();break;
    case 3:
    check_appointment();break;
    case 4: 
    cancel_appointment();break;}
}while(choice>0&&choice<=4);
    while (head != NULL) {
        Appointment *tmp = head;
        head = head->next;
        free(tmp);
    }
    return 0;
}

