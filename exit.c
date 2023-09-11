#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#define MAX 50

int i=0;
//int bill;
//char payment;
key_t key;
int msgid;


struct msg_buffer
{
    long msg_type;
    char msg_text[100];
} msg_send, msg_recv;


void send_carno()
{
    
	msg_send.msg_type = 21;
	
    //gets(msg_send.msg_text);
	//printf("%lu \n",strlen(msg_send.msg_text));
	
	msgsnd(msgid, &msg_send, strlen(msg_send.msg_text), 0);
}

void recv_bill()
{
    msgrcv(msgid, &msg_recv, sizeof(msg_recv), 22, 0);
  
    // display the message recieved
    //printf("Data Received is : %s \n", msg_recv.msg_text);
  
    //msgctl(msgid, IPC_RMID, NULL);
}

void pment(){
    // calculate bill according to time used for perking
    int payment;
    printf("BILL: Rs.%s\n",msg_recv.msg_text);
    printf("does customer paid the bill?(1 for yes / 0 for no)\n");
    scanf("%d", &payment);
    if(payment==0){
    	printf("Payment status: NOT Paid\n");
        pment();
    }
    printf("Payment: Paid\n\n\n");
    return;
}

int main() {
    char car_num[20];
    
    key = ftok("Parking", 130);
    msgid = msgget(key, 0666 | IPC_CREAT);
    
    while(1){
        
        printf("Enter car nuber: ");
        scanf("%s",car_num);
        strcpy(msg_send.msg_text , car_num);
        printf("car_num:%s\n",msg_send.msg_text);
        send_carno();
        
        sleep(1);
        printf("Checking alloted slot...\n");
        sleep(1);
        printf("Getting Bill...\n");
        
        recv_bill();
        //printf("Bill: Rs.%d \n",bill);
        //sleep(1);
        pment();
        //break;
        }
    return 0;
}






