#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#define MAX 50

int i=0;
static key_t key;
static int msgid;

struct msg_buffer
{
    long msg_type;
    char msg_text[100];
} msg_send, msg_recv;

/*void* wait()
{
    printf("Waiting ...\n");
}

void waiting()
{
    pthread_t t1, t2, t3, t4, t5;
    pthread_create(&t1, NULL, &wait, NULL); sleep(1);
    pthread_create(&t2, NULL, &wait, NULL); sleep(1);
    pthread_create(&t3, NULL, &wait, NULL); sleep(1);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
}*/

void req_slot(char *car_num)
{
    
	msg_send.msg_type = 11;
	strcpy(msg_send.msg_text, car_num);
    //gets(msg_send.msg_text);
	//printf("%lu \n",strlen(msg_send.msg_text));
	
	msgsnd(msgid, &msg_send, strlen(msg_send.msg_text), 0);
}

void get_response()
{
	strcpy(msg_recv.msg_text, "  \0");

    msgrcv(msgid, &msg_recv, sizeof(msg_recv), 12, 0);
  
    printf("Data Received is : %s \n", msg_recv.msg_text);
    //strcpy(msg_recv.msg_text, '\0');
    //msgctl(msgid, IPC_RMID, NULL);
}

void cur_time()
{
    char buffer[25];
        time_t current_time;
        struct tm *time_info;
    
        time(&current_time);
        time_info = localtime(&current_time);
    
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_info);
        printf("Current date and time: %s\n", buffer);
}

void decision(){
    if(msg_recv.msg_text[0]=='-')
        printf("No Slot Available!!!\nWait for some time...\n");
        
    else{
        printf("Alloted Parking Slot: %s\n",msg_recv.msg_text);
        //sleep(2);
        cur_time();
    }
}

void print_pricing()
{
    printf("/////////////////Parking (CAR ONLY!!!)/////////////\n");
    printf("/////////////////Rs. 40/- per hour/////////////////\n");
    printf("/////////////////Mininum charges: Rs.10////////////\n\n");
}

int main() {
    char car_num[20];
	key = ftok("Parking", 65);
    msgid = msgget(key, 0666 | IPC_CREAT);

    while(1){
        //waiting();
        print_pricing();
        printf("Enter car nuber: ");
        scanf("%s",car_num);
        //printf("Your Car nuber: %s\n",car_num);
        
        sleep(1);
        printf("Checking Availability...\n");
        
        printf("Requesting for nearest parking slot...\n");
        
        req_slot(car_num);
        get_response();
        //printf("%s \n",msg_send.msg_text);
        //sleep(1);
        decision();
        printf("\n\n\n");
        //break;
        }
    return 0;
}




