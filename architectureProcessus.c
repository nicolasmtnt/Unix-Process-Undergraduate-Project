#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define N 6 // nombre de process / acteur

float PaletteSize = 50*0.2*0.2;
float palettePrice = 5.99;



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
void requestPaletSize(Process from, Process to,int fd[N*N][2],float area, int *quantity,float *price);

void processRequest(char str[],int fd[2*N*N][2],Process from, Process to);
void respondAvailableArea(char content[],int fd[2*N*N][2],Process from, Process to);
void respondPaletSize(char content[],int fd[2*N*N][2],Process from, Process to);
int SurfacetoQuantity(float surface);

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

        serverloop(sw1,fd);
        
        closePipes(sw1,fd);
    }

    if(isProcess(sw2, pid)){ 
        //printf("Hello c'est le proccess sw2 qui vous parle \n");
        serverloop(sw2,fd);

        closePipes(sw2,fd);
    }

    if(isProcess(Antoine, pid)){ 
        //printf("Hello c'est le proccess Antoine qui vous parle \n");

        int x = requestAvailableArea(Antoine,sw1,fd, "creux");
        printf("la taille dispo est %d\n",x);




    }

    if(isProcess(Francoise, pid)){ 
        //printf("Hello c'est le proccess Francoise qui vous parle \n");
        
        int quantity;
        float price;
        requestPaletSize(Francoise,sw1,fd,20,&quantity,&price);
        closePipes(Francoise,fd);

    }

    if(isProcess(Jule, pid)){ 
        //printf("Hello c'est le proccess Jule qui vous parle \n");
        
        int x = requestAvailableArea(Jule,sw1,fd, "plein");
        printf("la surface disponible de palette plein est %d\n",x);

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
void serverloop(Process to,int fd[2*N*N][2]){
    char str[20];
    while(true)
        for(Process from = Antoine; from<N;from++)
            if(from != to){
                readString(fd[findAdress(from,to)],str);
                processRequest(str,fd,from,to);
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

void requestPaletSize(Process from, Process to,int fd[N*N][2],float area, int *quantity,float *price){
    char str[20] = "2,";
    char str2[20];
    char str3[20];
    char sarea[20];
    sprintf(sarea, "%.2f", area);
    strcat(str,sarea);

    writeString(fd[findAdress(from,to)],str);
    readString(fd[findAdress(to,from)],str2);
    readString(fd[findAdress(to,from)],str3);
    fprintf(stderr,"Vous pouvez acheter %s palettes aux prix de %s€ pour une surface de %.2f m\n",str2,str3,area);
    *quantity = atoi(str2);
    *price = atof(str3);

}


// ------- Responss of Process -------
void processRequest(char str[], int fd[2*N*N][2], Process from, Process to){
    int index = atoi(strtok (str,","));
    char *content = strtok (NULL,"");
    switch(index){
        case 1:
            respondAvailableArea(content,fd,from,to);
            break; 
        case 2:
            respondPaletSize(content,fd,from,to);
            break;
        default:
            break;
    }
}

void respondAvailableArea(char content[],int fd[2*N*N][2],Process from, Process to){
    char response[20];
    if(strcmp(content,"creux")==0)
        sprintf(response, "%d", surfaceDispoCreux);
    else if (strcmp(content,"plein")==0) 
        sprintf(response, "%d", surfaceDispoPlein);
    writeString(fd[findAdress(to,from)],response);
}

void respondPaletSize(char content[],int fd[2*N*N][2],Process from, Process to){
    char response1[20];
    char response2[20];
    int quantity = SurfacetoQuantity(atof(content));
    float price = palettePrice*quantity;
    sprintf(response1, "%d", quantity);
    sprintf(response2, "%.2f", price);
    writeString(fd[findAdress(to,from)],response1);
    writeString(fd[findAdress(to,from)],response2);
}

int SurfacetoQuantity(float surface){
    double quantity = surface/(float)PaletteSize;
    if(floorf(quantity) == quantity)
        return (int)quantity;
    else
        return (int)quantity+1;
}