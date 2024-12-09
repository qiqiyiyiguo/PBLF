#include<stdio.h>
#include<string.h>
#include<stdlib.h> 
#include<stdbool.h>
#include<ctype.h>
#define LEN 20
#define NUMBER 12
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
  int arrayMonth={1,2,3,4,5,6,7,8,9,10,11,12};
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
   
     

        	

