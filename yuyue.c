#include<stdio.h>
#include<string.h>
#include<stdlib.h> 
#include<stdbool.h>
#include<ctype.h>
#define LEN 20
#define NUMBER 12
#define MAX_SPOTS 100
#define MAX_NAME_LEN 50
#define MAX_USERS 100
#define IDNUMBER 6


typedef struct node{
  	char userName[LEN];
  	int startHour;
  	int endHour;
  	int month;
	int day;
	int year; 
  	int phonenumber[NUMBER];
  	struct node *next;
  }Appointment;
  
  Appointment *head;
  
  
void fun(char *p){
	int k=0;
	while(*p){
	if(k==0&&*p!=' '){
	*p=toupper(*p);
	k=1; 
	}else if(*p!=' '){
		k=1;
	}else{
		k=0;
	}
	*p++;
	}
}

Bool is_leapYear(int year){
	if(year%4==0&&year%400==0)
	return ture;
	else 
	return false;
}
void registerUser(User *p){     //注册用户 
	if(userNumber>=MAX_NUMBERS){
		printf("Users limit reached.\n");
		return;
	}
	printf("Enter your name:");
	scanf("%s",p->user_name);
	printf("\n");
    printf("Enter your ID:");
    scanf("%d",p->user_id);
    userNumber++;
    printf("User registered successfully.\n");
}

void insert_appointment(Appointment *p){    //预约 
	Appointment2 *head=(Appointment2 *)malloc(sizeof(Appointment));
	if(head==NULL){
		printf("The booking is invalid!");
		return;
	}
	char name[LEN];int a,b,c,d,e;
	long int f;
	scanf("%s %d %d %d %d %d %ld",name,&a,&b,&c,&d,&e,&f);
	*p={name,a,b,c,d,e,f};
	if(a<8||b>=18||c<1||c>12||(f!=2024&&f!=2025)){
		printf("The booking is unsuccessful!\n");
		return;}
	if(c==1||c==3||c==5||c==7||c==8||c==10||c==12){
    if(d<1||d>31){
     printf("The booking is unsuccessful!\n");
     return;
	}else
	printf("The booking is successful!\n");
	return;}
	else if(c==4||c==6||c==9||c==11){
	if(d<1||d>30){
     printf("The booking is unsuccessful!\n");
     return;}
     else printf("The booking is successful!\n");
     return;}
     else {
      if(is_leapYear(f)){
      	if(day<1||day>29){
      	printf("The booking is unsuccessful!\n");
      	return;}
      	else {printf("The booking is successful!\n");
      	return;}
	  }
	  else {
	  	if(day<1||day>28){
	  	printf("The booking is unsuccessful!\n");
	  	return;}
	  	else {printf("The booking is successful!\n");
	  	return;}
	  } 
	 }

								   
}

void check_appointment(Appointment *p,int month,int day,int year){   //检查预约是否成功 

	if(p->month>=1&&p->month<=12&&startHour>=8&&p->startHour<=16&&p->endHour>=9&&p->endHour<=17&&p->day){
		if(p->month==4||p->month==6||p->month==9||p->month==11){
			if(p->day>=1&&p->day<=30){
			printf("The booking is successful!\n");
			return;}
			else{printf("The booking is unsuccessful!\n");
	  	    return;}
			}
		else if(p->month==1||p->month==3||p->month==5||p->month==7||p->month==8||p->month==10||p->month==12){
			if(p->day>=1&&p->day<=31){
			printf("The booking is successful!\n");
			return;}
			else{printf("The booking is unsuccessful!\n");
	  	    return;}	
			}
		else {
			if(is_leapYear(year)){
			if(p->day>=1&&p->day<=29){
			printf("The booking is successful!\n");
			return;}
			else{printf("The booking is unsuccessful!\n");
	  	    return;}		
			}
			else{
			if(p->day>=1&&p->day<=28){
			printf("The booking is successful!\n");
			return;}
			else{printf("The booking is unsuccessful!\n");
	  	    return;}		
			}
		}
	}	
}

bool arraysEqual(int arr1[],int arr2[]){  //judge arrays are equal
	int len1=strlen(arr1);
	int len2=strlen(arr2);
	if(len1!=len2){
	return false;}
	for(int i=0;i<len1;i++){
	if(arr1[i]!=arr2[i]){
	return false;}
	}
	return true;
}  

void cancel_appointment(Appointment *p){    //取消预约 
	 int i;
	 Appointment *tmp=(Appointment *)malloc(sizeof(Appointment));
	 Appointment *new_node=(Appointment *)malloc(sizeof(Appointment));
	 if(tmp==NULL){
	 printf("The data is invalid!\n");
	 return; 
	 }
	 if(new_node==NULL){
	 printf("The data is invalid!\n");
	 return; 
	 }
	 tmp->next=head;
	 head=tmp;
	 int phone_number[NUMBER];
	 gets(phone_number);
	 for(i=0;i<100;i++){  //通过电话查找 
	 if(arraysEqual(tmp->phonenumber,phone_number)){
	 	printf("The arrays are equal!");
	 	break;
	 }  if(tmp->next==NULL){
	 	printf("The booking is unsuccessful!");
	 	return;
	 }  new_node=tmp;             //pay attention to the way of deleting the node
	 tmp=tmp->next;	
}    
new_node->next=tmp->next;        //i--new_node,i+1--tmp;
free(tmp);
} 


//分景点预约

	typedef struct {
	char spot_name[MAX_NAME_LEN];
	int booked;    
	int capacity;
	}Spot; 
    typedef struct{
  	char user_name[LEN];
	char user_id[IDNUMBER];
	}User;
	Spot spots[MAX_SPOTS];
	User user[MAX_USERS]; 
	int spotNumber=0;
	int userNumber=0;
	
void registerUser(){     //注册用户 
	if(userNumber>=MAX_NUMBERS){               //限制注册用户 
		printf("Users limit reached.\n");
		return;
	}
	printf("Enter your name:");
	scanf("%s",user->user_name);
	printf("\n");
    printf("Enter your ID:");
    scanf("%d",user->user_id);
    userNumber++;
    printf("User registered successfully.\n");
}

void add_spot(){                    //管理员 
	if(spotNumber>=MAX_SPOTS){
		printf("Sight limit reached.\n");
		return; 
	}
	printf("Enter sight name:");
	scanf("%s",spots[spotNumber].spot_name);
	printf("\n");
	printf("Enter the capacity of the campus:");
	scanf("%d",&spots[spotNumber].capacity);
	spots[spotNumber].booked=0;
	spotNumber++;
	printf("Sight added successfully.\n"); 
}

void spot_appointment(){      //客户端预约 
	char Sightname[LEN];
	long int ID;
	int i,IDfound=0,Sightfound=0;
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
	return;
} 
    printf("Enter the name of sight:");
    scanf("%s",Sightname);
    printf("\n");
   for(i=0;i<MAX_SPOTS;i++){
   	if(strcmp(spots->spot_name,Sightname)==0){
   	printf("Find the sight!\n");
   	Sightfound=1;
   	break;
	   }
   }
if(!Sightfound){
	printf("Not find the sight!\n");
	return;
}
    (spots->booked)++;
    printf("Booking the sight successfully!\n");
    
void checkBooking(){        //查询预约 
	char sightname[MAX_NAME_LEN];
	scanf("%s",sightname);
	int i;
	for(i=0;i<spotNumber;i++){
		if(strcmp(spots[i].spot_name,sightname)==0){
			printf("景点名称：%s ,预约状态:%s\n",spots[i].spot_name,
			spots[i].spot_name? "已预约":"未预约");
			printf("Booked: %d/%d\n",spots->booked,spots->capacity);
			return;
		}
	}
	printf("Not find the sight！");
}

void modify_spot(){                    //修改预约状态 
	char name[MAX_NAME_LEN];
	scanf("%s",name);
    int i;
    for(i=0;i<spotNumber;i++){
    	if(strcmp(spots[i].spot_name,name)==0){
    	printf("景点名称:%s ,预约状态：%s\n",spots[i].spot_name,
		spot[i].booked?"未预约":"已预约");  
		return; 
		}
	}
	printf("未找到该景点!");
}
   
     

        	

