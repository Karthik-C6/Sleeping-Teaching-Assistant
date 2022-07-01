#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#define seats 3
#define students 5

//function for ta and student actions
void* ta_actions();
void* student_action(void* id);

int Waiting(int id);
//semaphores
pthread_mutex_t mutex;
sem_t sem_ta;
sem_t sem_students;
int ta_flag=0;

//variables
int waiting_hall_seats[seats];
int students_waiting=0;
int teaching_position=0;
int seating_position=0;

int main(){
    
    pthread_t ta;
    pthread_t student[students];
    int student_ids[students];
    
    sem_init(&sem_ta,0,1);
    sem_init(&sem_students,0,0);
    
    //Creating Threads
    pthread_mutex_init(&mutex,NULL);
    pthread_create(&ta,NULL,ta_actions,NULL);
    for(int i=0;i<students;i++){
        student_ids[i]=i+1;
        pthread_create(&student[i],NULL,student_action,&student_ids[i]);
    }
    
    //Joining threads
    pthread_join(ta,NULL);
    for(int i=0;i<students;i++){
        pthread_join(student[i],NULL);
    }
    
    return 0;
}

void* ta_actions(){
    printf("Checking for students\n");
    while(1){
        if(students_waiting==0){
            if(ta_flag==0){
            printf("No students so TA is sleeping\n");
            ta_flag=1;
            }
        }
        else{
            ta_flag=0;
            sem_wait(&sem_students);
            pthread_mutex_lock(&mutex);
            
            int help_time = rand()%5;
            printf("TA helping student%d for %d seconds\n",waiting_hall_seats[teaching_position],help_time);
            waiting_hall_seats[teaching_position]=0;
            students_waiting--;
            teaching_position = (teaching_position+1)%seats;
            sleep(help_time);
            pthread_mutex_unlock(&mutex);
            sem_post(&sem_ta);
        }
    }
    
}

void* student_action(void* id){
    int student_id=*(int*)id;
    while(1){
        if(Waiting(student_id)){continue;}
        
        int time=rand()%5;
        printf("Student %d is comming to ask help for TA in %d seconds\n",student_id,time);
        sleep(time);
        
        pthread_mutex_lock(&mutex);
        if(students_waiting==seats){
            printf("Student %d has no seat to wait please come after some time\n",student_id);
        }
        else{
            waiting_hall_seats[seating_position]=student_id;
            students_waiting++;
            printf("Student %d take seat no %d in waiting hall\n",student_id,seating_position+1);
            seating_position=(seating_position+1)%seats;
            sem_post(&sem_students);
            sem_post(&sem_ta);
        }
        pthread_mutex_unlock(&mutex);
    }
}

int Waiting(int id){
    int temp=0;
    for(int i=0;i<seats;i++){
        if(waiting_hall_seats[i]==id)
            temp=1;
    }
    return temp;
}
