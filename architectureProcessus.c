#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define N 6 // nombre de process / acteur

typedef enum {
    sw1, sw2, Antoine, Francoise, Jule, Anne
} Process;

typedef enum {
    false,true
} bool;

int surfaceDispoCreux = 100;
int surfaceDispoPlein = 200;

// Les prototypes
void error(const char *msg);
int sumArray(int array[],int n);
void printArray(int array[], int n);
int isProcess(Process id,int pid[]);
void setPipes(Process id, int fd[N*N][2]);
void closePipes(Process id, int fd[N*N][2]);

void writeString(int fd[],char str[]);
void readString(int fd[],char str[]);

void serverloop(Process id,int fd[2*N*N][2]);
void detectQuery(int fd[2*N*N][2],char str[],int i);
void browseOptions(char str[]);
int findAdress(Process from, Process to);
int requestAvailableArea(Process from, Process to,int fd[N*N][2],char type[]);
void processRequest(char str[],int fd[2*N*N][2],Process from, Process to);


int main(void){

    
    int pid[N] = {0};
    int fd[2*N*N][2]; // Instanciation des files directories

    for(int i = 0; i< 2*N*N;i++){ // Creation des pipes
        if(pipe(fd[i]) < 0) 
            error("An error occured during pipes creation");
    }
    for(int i=0; i<N; i++){ // creation des processus
        if(sumArray(pid,N-1)==0) 
            pid[i] = fork(); 
    }
    
    //  ------------ LES PROCESS ------------

    if(isProcess(sw1, pid)){ 
        //printf("Hello c'est le proccess sw1 qui vous parle \n");

        char str[200];
        readString(fd[findAdress(Antoine,sw1)],str);
        processRequest(str, fd, Antoine,sw1);
        
        closePipes(sw1,fd);
    }

    if(isProcess(sw2, pid)){ 
        //printf("Hello c'est le proccess sw2 qui vous parle \n");
        setPipes(sw2,fd);

        closePipes(sw2,fd);
    }

    if(isProcess(Antoine, pid)){ 
        //printf("Hello c'est le proccess Antoine qui vous parle \n");


        int x = requestAvailableArea(Antoine,sw1,fd, "creux");
        printf("%d",x);

        closePipes(Antoine,fd);
    }

    if(isProcess(Francoise, pid)){ 
        //printf("Hello c'est le proccess Francoise qui vous parle \n");
        
        closePipes(Francoise,fd);
    }

    if(isProcess(Jule, pid)){ 
        //printf("Hello c'est le proccess Jule qui vous parle \n");


        closePipes(Jule,fd);
    }

    if(isProcess(Anne, pid)){ 
        //printf("Hello c'est le proccess Anne qui vous parle \n");

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

void setPipes(Process id, int fd[2*N*N][2]){
    int min = 6*id;
    int max = 6*id + 6;
    for(int i = 0; i<min; i++)
        close(fd[i][1]);
    for(int i = max; i<N*N;i++)
        close(fd[i][1]);
    for(int i = min; i< max;i++)
        close(fd[i][0]);
    

    int rmin = min + (N)*N;
    int rmax = max + (N)*N;
    for(int i = N*N; i<rmin; i++)
        close(fd[i][0]);
    for(int i = rmax; i<2*N*N;i++)
        close(fd[i][0]);
    for(int i = rmin; i< rmax;i++)
        close(fd[i][1]);
}

void closePipes(Process id, int fd[2*N*N][2]){
    int min = 6*id;
    int max = 6*id + 6;
    for(int i = 0; i<min; i++)
        close(fd[i][0]);
    for(int i = max; i<N*N;i++)
        close(fd[i][0]);
    for(int i = min; i< max;i++)
        close(fd[i][1]);
    
    int rmin = min + (N)*N;
    int rmax = max + (N)*N;
    for(int i = N*N; i<rmin; i++)
        close(fd[i][1]);
    for(int i = rmax; i<2*N*N;i++)
        close(fd[i][1]);
    for(int i = min; i< max;i++)
        close(fd[i][0]);
    
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

// ETAPE 2
void serverloop(Process id,int fd[2*N*N][2]){
    int minforward = 6*id;
    int maxforward = 6*(id+1);
    int minbackward = minforward + N*N;
    int maxbackward = maxforward + N*N;
    char str[200];
    while(true){
        
        usleep(500000);
        for(int i=0;i<minforward;i++){
            detectQuery(fd,str,i);
        }
        for(int i=maxforward;i<minbackward;i++){
            detectQuery(fd,str,i);
        }
        for(int i=maxbackward; i<N*N; i++){
            detectQuery(fd,str,i);
        }
    }
}

void detectQuery(int fd[2*N*N][2],char str[],int i){
    readString(fd[i],str);
    printf("%s",str);
    fflush(stdout);
    if(strcmp(str,"") !=0){
        browseOptions(str);
    }
}

void browseOptions(char str[]){
    printf("%s",str);
}



int findAdress(Process from, Process to){
    if(from<to)
        return 6*from+to;
    else
        return N*N+6*from+to;
}

int requestAvailableArea(Process from, Process to,int fd[N*N][2],char type[]){
    char str[20] = "1,";
    strcat(str,type);
    writeString(fd[findAdress(from,to)],str);
    readString(fd[findAdress(to,from)],str);
    return atoi(str);

}

void processRequest(char str[], int fd[2*N*N][2], Process from, Process to){
    int index = atoi(strtok (str,","));
    char *content = strtok (NULL,"");
    switch(index){
        case 1:
            if(strcmp(content,"creux")==0){
                char response[20];
                sprintf(response, "%d", surfaceDispoCreux);
                writeString(fd[findAdress(to,from)],response);
            }

            break;
        default:
            break;
    }
}
