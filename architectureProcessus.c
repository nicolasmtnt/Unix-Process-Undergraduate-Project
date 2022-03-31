#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


char processes[][20] = {"sw1","sw2","Antoine","Francoise","Jule","Anne"};
const int N = sizeof(processes)/sizeof(processes[0]); // N = 6


// Les prototypes
void error(const char *msg);
int sumArray(int array[],int n);
void printArray(int array[], int n);
int isProcess(char str[],int pid[]);

int main(void){
    
    int pid[N] = {0};
    int fd[N][2]; // Instanciation des files directories
    for(int i=0; i<N; i++){
        if(pipe(fd[i]) < 0) // Creation des pipes
            error("An error occured during pipes creation");
        if(sumArray(pid,N-1)==0)
            pid[i] = fork(); // creation des processus
    }
    
    // verification  avec des examples ci dessous
    if(isProcess("Anne", pid)){ 
        printf("Hello c'est le proccess Anne qui vous parle \n");
    }

    printArray(pid,N);

    return 0;
}



void error(const char *msg){
  perror(msg);
  exit (-1);
}

int sumArray(int array[],int n){
    int sum = 0;
    for(int i =0; i<n; i++)
        sum += array[i];
    return sum;
}

void printArray(int array[], int n){
    for(int i =0; i<n; i++)
        printf("[%d] ",array[i]);
    printf("\n");
}


int isProcess(char str[],int pid[]){
    for(int i = 0; i<N; i++)
        if(pid[i] != 0 && strcmp(processes[i],str)==0)
            return 1;
    return 0;
}