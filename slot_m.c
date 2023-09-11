// slot management 

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#define MAX 50
int i=0;
int n=1;
int alloted_slot;

struct msg_buffer
{
    long msg_type;
    char msg_text[100];
}
msg_sendp, msg_sendc,
msg_recvp, msg_recvc;

key_t key_p, key_c, key_shm;		//for message queues and shared memory
int msgidp;
int msgidc;

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////BASIC OPERATIONS///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

char* cur_time()
{
    char c_time[30];
    time_t current_time;
    struct tm *time_info;
    
    time(&current_time);
    time_info = localtime(&current_time);
    
    strftime(c_time, sizeof(c_time), "%Y-%m-%d %H:%M:%S", time_info);
    printf("Current date and time: %s\n", c_time);
    return c_time;
}

////////////////////////////////////////////////////////////////////////////////

int char_to_int(char *chr)
{
    int converted_int = 0; 
    int i = 0; 
    
    while (chr[i] != '\0') {
        converted_int = converted_int * 10 + (chr[i] - '0');
        i++;
    }
    return converted_int;
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////LINKED LIST OPERATIONS/////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct p_slots
{
    int status;
    int node_val;
    int slot_no;
    char p_time[30];
    char car_no[12];
    struct p_slots *next;
} *buff;

struct p_slots *head[10] = {NULL};

void dist(struct p_slots *new) //this function will define distance netween entry point to an individual slot for each slot
{
    if(n<=5)
        new->node_val=n;
    else if(n>5 && n<=10)
    {
        new->node_val=n%5 +1;
        if(n%5==0)
            new->node_val=5 +1;
    }
    else if(n>10 && n<=15)
    {
        new->node_val=n%5 +2;
        if(n%5==0)
            new->node_val=5 +2;
    }
    else if(n>15 && n<=20)
    {
        new->node_val=n%5 +3;
        if(n%5==0)
            new->node_val=5 +3;
    }
    else if(n>20 && n<=25)
    {
        new->node_val=n%5 +4;
        if(n%5==0)
            new->node_val=5 +4;
    }
    else if(n>25 && n<=30)
    {
        new->node_val=n%5 +5;
        if(n%5==0)
            new->node_val=5 +5;
    }
    else if(n>30 && n<=35)
    {
        new->node_val=n%5 +6;
        if(n%5==0)
            new->node_val=5 +6;
    }
    else if(n>35 && n<=40)
    {
        new->node_val=n%5 +7;
        if(n%5==0)
            new->node_val=5 +7;
    }
    else if(n>40 && n<=45)
    {
        new->node_val=n%5 +8;
        if(n%5==0)
            new->node_val=5 +8;
    }
    else if(n>45 && n<=50)
    {
        new->node_val=n%5 +9;
        if(n%5==0)
            new->node_val=5 +9;
    }
}

void create_parking_slots(struct p_slots **head)
{
    // Create 10 linked lists containing 5 nodes each with slot_no ranging from 1 to 5 for first linked list,
    // 6 to 10 for second linked list, and so on
    for (int i = 0; i < 10; i++) {
        struct p_slots *prev = NULL;
        for (int j = 0; j < 5; j++) {
            struct p_slots *new_node = (struct p_slots *) malloc(sizeof(struct p_slots));
            new_node->status = 0;
            new_node->slot_no = n;
            dist(new_node);
            n++;
            //printf("assigned slot: %d\n",new_node->slot_no);
            strcpy(new_node->car_no, "");
            strcpy(new_node->p_time, "");
            new_node->next = NULL;

            // Insert the new node at the end of the linked list
            if (head[i] == NULL) {
                head[i] = new_node;
                prev = new_node;
            } else {
                prev->next = new_node;
                prev = new_node;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////FOR PARENT PROCESS///////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void get_req()
{
    strcpy(msg_recvp.msg_text, "            ");
    msgrcv(msgidp, &msg_recvp, sizeof(msg_recvp), 11, 0);
  
    printf("Data Received %s \n", msg_recvp.msg_text);
  
    //msgctl(msgid, IPC_RMID, NULL);
}
////////////////////////////////////////////////////////////////////////////////
void get_nslot()
{
    int min_val = 100;
    int slot = -1;
    
    time_t current_time;
    struct tm *time_info;
    time(&current_time);
    time_info = localtime(&current_time);

    // Iterate over all the nodes in the 10 linked lists to find the node with the lowest node_val and status=0
    for (int i = 0; i < 10; i++) {
        struct p_slots *current = head[i];
        while (current != NULL) {
            if (current->status == 0 && current->node_val < min_val) {
                min_val = current->node_val;
                slot = current->slot_no;
            }
            current = current->next;
        }
    }
	//char null[10]= "abc";
	//char gv[12];
    // If a slot with status=0 was found, update its status to 1
    if (slot != -1) {
        for (int i = 0; i < 10; i++) {
            struct p_slots *current = head[i];
            while (current != NULL) {
                if (current->slot_no == slot) {
                    current->status = 1;
                    //printf("car no.: %s\n", current->car_no);
                    strcpy(current->car_no, msg_recvp.msg_text);
                    //strcpy(gv, null);
                    //printf("gv: %s",gv);
                    printf("car no.: %s\n", current->car_no);
                    strftime(current->p_time, sizeof(current->p_time), "%Y-%m-%d %H:%M:%S", time_info);
            		printf("Slot Allotment Time: %s\n",current->p_time);
                    break;
                }
                //printf("Parking Time: %s\n",current->p_time);
                current = current->next;
            }
       
        }
    }
    sprintf(msg_sendp.msg_text, "%d", slot);
    //printf("Slot no. %s alloted\n",msg_sendp.msg_text);
}
////////////////////////////////////////////////////////////////////////////////
void snd_slot()
{
	msg_sendp.msg_type = 12;
	
    //gets(msg_sendp.msg_text);
	//printf("%lu \n",strlen(msg_sendp.msg_text));
	
	msgsnd(msgidp, &msg_sendp, strlen(msg_sendp.msg_text), 0);
	printf("Slot no. %s alloted\n",msg_sendp.msg_text);
	strcpy(msg_sendp.msg_text, "          \0");
}
////////////////////////////////////////////////////////////////////////////////
void p_linkedl()
{
for (int i = 0; i < 10; i++) {
        struct p_slots *current = head[i];
        
        while (current != NULL) {
        	printf("-->%s",current->car_no);
        	current = current->next;
		}
		printf("\n");
		
	}
}
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////FOR CHILD PROCESS////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void get_carn()
{
    strcpy(msg_recvc.msg_text, "            ");
    msgrcv(msgidc, &msg_recvc, sizeof(msg_recvc), 21, 0);
  
    // display the message recieved
    printf("Data Received is : %s \n", msg_recvc.msg_text);
    
  
    //msgctl(msgid, IPC_RMID, NULL);
}
////////////////////////////////////////////////////////////////////////////////
void fetch_slot()
{
    //int slot = -1;
    // Iterate over all the nodes in the 10 linked lists to find the node with the lowest node_val and status=0
    
    for (int i = 0; i < 10; i++) {
        struct p_slots *current = head[i];
        //printf("Car no : %s \n",current->car_no );
        
        while (current != NULL) {
        	//printf("Car no : %s \n",current->car_no );
            
            //printf("%d \n", strcmp(current->car_no, msg_recvc.msg_text));
            
            if (strcmp(current->car_no, msg_recvc.msg_text) == 0) {
                //printf("%d \n", strcmp(current->car_no, msg_recvc.msg_text));
                buff = current;
                strcpy(current->car_no , "            ");
                current->status = 0;
                printf("Emptied slot no. %d\n",current->slot_no);
                //break;
                //slot = current->slot_no;
            }
            current = current->next;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
void calc_bill()
{
	printf("p_time: %s\n", buff->p_time);
    char buffer[30];
    ////////////////////////////////////////////////////////////////////////////
    //char c_time[30];
    time_t current_time;
    struct tm *time_info;
    
    time(&current_time);
    time_info = localtime(&current_time);
    
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_info);
    printf("Current date and time: %s\n", buffer);
    ////////////////////////////////////////////////////////////////////////////
    printf("c_time: %s\n", buffer);
    
    // Convert the first date-time string to a time_t object
    struct tm tm1 = {0};
    strptime(buffer, "%Y-%m-%d %H:%M:%S", &tm1);
    time_t c_time = mktime(&tm1);
    printf("c_time: %s\n", buffer);

    // Convert the second date-time string to a time_t object
    struct tm tm2 = {0};
    strptime(buff->p_time , "%Y-%m-%d %H:%M:%S", &tm2);
    time_t p_time = mktime(&tm2);
    printf("p_time: %s\n", buff->p_time);

    // Calculate the difference in seconds
    int diff_seconds = difftime(c_time, p_time);
    // Convert to minutes
    int diff_minutes = diff_seconds / 60;
    printf("Time diff: %d\n",diff_minutes);
    
    float bill = 0;
    bill = diff_minutes/15.0 * 10;
    if(bill < 10)
        bill=10;
    
    sprintf(msg_sendc.msg_text, "%f", bill);
}
////////////////////////////////////////////////////////////////////////////////
void snd_bill()
{
	msg_sendc.msg_type = 22;
	
    //msg_sendc.msg_text;
	printf("%s\n",(msg_sendc.msg_text));
	
	msgsnd(msgidc, &msg_sendc, strlen(msg_sendc.msg_text), 0);
	strcpy(msg_sendc.msg_text, "          \0");
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////Process Calling functions///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void *entryf(void *arg)
{
	while(1){
		//get request
		get_req();
		//searching for slot
		get_nslot();
		//send slot
		snd_slot();
		//print linkedL
		p_linkedl();
    }
    pthread_exit(0);
    
}
////////////////////////////////////////////////////////////////////////////////
void *exitf(void *arg)
{
	while(1){
		//p_linkedl();
		//get car number
		get_carn();
		
		//p_linkedl();
		//fetching slot
		fetch_slot();
		//calculating bill from slot data
		calc_bill();
		//send details
		snd_bill();
		//print linkedL
		p_linkedl();
    }
    pthread_exit(0);
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main()
{

    
    // Create parking slots
    create_parking_slots(head);
    printf("Initiated Parking slots\n");
    
    key_p = ftok("Parking", 65) ;

    msgidp = msgget(key_p, 0666 | IPC_CREAT);
    
    key_c = ftok("Parking", 130) ;
    
    msgidc = msgget(key_c, 0666 | IPC_CREAT);
    ////////////////////////////////////////////////////////////////////////////
    pthread_t tid1, tid2;
    
    pthread_create(&tid1, NULL, entryf, NULL);
    pthread_create(&tid2, NULL, exitf, NULL);
    
    pthread_join(&tid1, NULL);
    pthread_join(&tid2, NULL);
    ////////////////////////////////////////////////////////////////////////////
    /*int shmid;
    key_shm = ftok("Parking", 260) ;
    shmid = shmget(key_shm, (50*sizeof(struct p_slots)), IPC_CREAT|0666);
    **head = (struct p_slots**) shmat(shmid, (struct p_slots**)0,0);				//in 2nd arg, we put 0 so that kernel can give an address by itself
    ////////////////////////////////////////////////////////////////////////////
    
    int id=fork();
    if(id)
    {
        while(1)
        {
            //parent process
            entryf();
        }
    }
    else
    {
        while(1)
        {
            //child process
            exitf();
        }
    }*/
    return 0;
}

