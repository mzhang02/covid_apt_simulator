#include <semaphore.h>
#include <pthread.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h> 
#include <stdio.h>

static sem_t connected_lock; //binary semaphone
static sem_t operators; //counting semaphone
static sem_t id_lock; //binary semaphone
static int NUM_OPERATORS = 4;
static int NUM_LINES = 7;
static int connected = 0; //callers that are connected
static int next_id = 1; //ID count

void* phonecall(void* vargp);

//command line args: number of args, list of args
int main (int argc, char **argv){
    //error catching
    if (argc != 2){
        printf("No input or wrong number of args: please use non-negative COVID_test <num_of_callers>\n");
        return -1;
    }
    int num_of_callers = atoi(argv[1]); //take in user input
    if (num_of_callers == 0){
        printf("Irregular input (char, string, or '0' input): please use non-negative COVID_test <num_of_callers>\n");
        return -1;
    }
    //malloc array of threads, create threads for each phone call
    printf("Start handling calls:\n");
    pthread_t *tid = malloc(num_of_callers * sizeof (pthread_t));
    //connected_lock semaphone initialization
    sem_init(&connected_lock, 0, 1);
    //operator semaphone initialization
    sem_init(&operators, 0, NUM_OPERATORS);
    //next_id semaphone initialization
    sem_init(&id_lock, 0, 1);

    //pthread_create, join 
    for (int i = 0; i < num_of_callers; i++){
        pthread_create(tid + i, NULL, &phonecall, NULL); //tid + i is address
    }
    for (int j = 0; j < num_of_callers; j++){
        pthread_join(tid[j], NULL); //tid[i] is value
    }
    printf("All calls have been handled!\n");
    free(tid);
    sem_destroy(&connected_lock);
    sem_destroy(&operators);
    sem_destroy(&id_lock);
    return 0;
}

void* phonecall(void* vargp){
    //connecting a caller to a line
    sem_wait(&id_lock);
    int id = next_id;
    next_id++;
    sem_post(&id_lock);
    int new_caller = 1; //flag to see if caller has been waiting or not
    printf("Thread %d is attempting to connect...\n", id);
    while(1){
        sem_wait(&connected_lock);
        if (connected == NUM_LINES){ //busy line
            sem_post(&connected_lock);
            if (new_caller){
                printf("Thread %d is busy, please hold.\n", id);
                new_caller = 0;
            }
            sleep(1);
        }
        else{ //free line
            //update connected
            connected++;
            sem_post(&connected_lock);
            printf("Thread %d connects to an available line, call ringing...\n", id);
            break;
        }
    }

    //connecting a caller to an operator
    sem_wait(&operators);
    printf("Thread %d is speaking to an operator in the local health center.\n", id);
    sleep(3); // taking medicine
    printf("Thread %d has made an appointment for the test! The operator has left...\n", id);
    sem_post(&operators);

    //update connected, hang up
    sem_wait(&connected_lock);
    connected--;
    sem_post(&connected_lock);
    printf("Thread %d has hung up!\n", id);
    return NULL;
}
