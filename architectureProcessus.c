#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define N 6 // nombre de process / acteur

typedef enum {
    sw1, sw2, Antoine, Francoise, Jule, Anne
} Process;

// Les prototypes
void error(const char *msg);
int sumArray(int array[],int n);
void printArray(int array[], int n);
int isProcess(Process id,int pid[]);
void setPipes(Process id, int fd[N*(N-1)][2]);
void closePipes(Process id, int fd[N*(N-1)][2]);

void writeString(int fd[],char str[]);
void readString(int fd[],char str[]);


int main(void){
    
    int pid[N] = {0};
    int fd[2*N*(N-1)][2]; // Instanciation des files directories

    for(int i = 0; i< N*(N-1);i++){ // Creation des pipes
        if(pipe(fd[i]) < 0) 
            error("An error occured during pipes creation");
    }
    for(int i=0; i<N; i++){ // creation des processus
        if(sumArray(pid,N-1)==0) 
            pid[i] = fork(); 
    }
    
    if(isProcess(sw1, pid)){ 
        //printf("Hello c'est le proccess sw1 qui vous parle \n");
        setPipes(sw1,fd);

//-----
        writeString(fd[0],"hello");
//------
        closePipes(sw1,fd);
    }

    if(isProcess(sw2, pid)){ 
        //printf("Hello c'est le proccess sw2 qui vous parle \n");
        setPipes(sw2,fd);

        closePipes(sw2,fd);
    }

    if(isProcess(Antoine, pid)){ 
        //printf("Hello c'est le proccess Antoine qui vous parle \n");
        setPipes(Antoine,fd);

// ------
        int n;
        char str[200];
        readString(fd[0],str);
        printf("%s",str);
// ------

        closePipes(Antoine,fd);
    }

    if(isProcess(Francoise, pid)){ 
        //printf("Hello c'est le proccess Francoise qui vous parle \n");
        setPipes(Francoise,fd);
        
        closePipes(Francoise,fd);
    }

    if(isProcess(Jule, pid)){ 
        //printf("Hello c'est le proccess Jule qui vous parle \n");
        setPipes(Jule,fd);

        closePipes(Jule,fd);
    }

    if(isProcess(Anne, pid)){ 
        //printf("Hello c'est le proccess Anne qui vous parle \n");
        setPipes(Anne,fd);

        closePipes(Anne,fd);
    }
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


int isProcess(Process id,int pid[]){
    for(int i = 0; i<N; i++)
        if(pid[i] != 0 && id ==i)
            return 1;
    return 0;
}

void setPipes(Process id, int fd[N*(N-1)][2]){
    int min = 6*id;
    int max = 6*id + 6;
    for(int i = 0; i<min; i++)
        close(fd[i][1]);
    for(int i = max; i<N*(N-1);i++)
        close(fd[i][1]);
    for(int i = min; i< max;i++)
        close(fd[i][0]);
}

void closePipes(Process id, int fd[N*(N-1)][2]){
    int min = 6*id;
    int max = 6*id + 6;
    for(int i = 0; i<min; i++)
        close(fd[i][0]);
    for(int i = max; i<N*(N-1);i++)
        close(fd[i][0]);
    for(int i = min; i< max;i++)
        close(fd[i][1]);
}


void writeString(int fd[],char str[]){

    int n = strlen(str)+1;
    write(fd[1],&n,sizeof(int));
    write(fd[1],str,sizeof(char)*n);
}

void readString(int fd[],char str[]){

    int n;
    read(fd[0],&n,sizeof(int));
    read(fd[0],str,sizeof(char)*n);
}