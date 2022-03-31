#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define N 2 // number of processes (N processes equivalent to N-1 fork)

void error(const char *msg);
int sumArray(int array[],int n);
void printArray(int array[], int n);
int arraysEquals(int array1[],int array2[],int n);
int isProcess(char str[],int pid[]);

int sw1[] = {1,0};
int sw2[] = {0,0};


int main(void){

    // int fd[N][2];
    // for(int i=0; i<N; i++)
    //     if(pipe(fd[i]) < 0)
    //         error("An error occured during pipes creation");
    
    int pid[N-1] = {0};
    for(int i=0; i<N-1; i++){
        if(sumArray(pid,N-1)==0){
            pid[i] = fork();
        }
    }

    return 0;
}



void error(const char *msg) {
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

int isProcess(char str[],int pid[]){ // ne fonctionne pas
    if(pid[0] != 0 && strcmp("sw1",str)){
        printf("hello");
        return 1;
    }
    return 0;
}