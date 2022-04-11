#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define N 6 // nombre de process / acteur

float PaletteSize = 50*0.2*0.2;
float palettePleinPrice = 5.99;
float paletteCreuxPrice = 4.99;



typedef enum {
    sw1, sw2, Antoine, Francoise, Jule, Anne
} Process;

typedef enum {
    false,true
} bool;

char cardNumberRegistry[2][20] = {
    "5412751234123456","4000123456789010"
};

char cardCryptoRegistry[2][20] = {
    "274","947"
};

float surfaceDispoCreux = 100;
float surfaceDispoPlein = 200;

// Les prototypes
void error(const char *msg);
int sumArray(int array[],int n);
void printArray(int array[], int n);
int isProcess(Process id,int pid[]);
void setPipes(Process id, int fd[N*N][2]);
void closePipes(Process id, int fd[N*N][2]);

void writeString(int fd[],char str[]);
void readString(int fd[],char str[]);

void detectQuery(int fd[2*N*N][2],Process from, Process to);
int findAdress(Process from, Process to);

float requestAvailableArea(Process from, Process to,int fd[N*N][2],char type[]);
void requestQuantityAndPrice(Process from, Process to,int fd[N*N][2],char type[], float area, int *quantity,float *price);
void requestPayment(Process from, Process to,int fd[N*N][2],char type[], int quantity, char cardNumber[16],short crypto);

void processRequest(char str[],int fd[2*N*N][2],Process from, Process to);
char* getName(Process user);
void respondAvailableArea(char content[],int fd[2*N*N][2],Process from, Process to);
void respondPaletSize(char content[],int fd[2*N*N][2],Process from, Process to);
int SurfacetoQuantity(float surface);
void respondPayment(char content[],int fd[2*N*N][2],Process from, Process to);

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

        detectQuery(fd,Antoine,sw1);
        detectQuery(fd,Francoise,sw1);
        
        closePipes(sw1,fd);
    }

    if(isProcess(sw2, pid)){ 
        //printf("Hello c'est le proccess sw2 qui vous parle \n");
        detectQuery(fd,Antoine,sw2);
        closePipes(sw2,fd);
    }

    if(isProcess(Antoine, pid)){ 
        //printf("Hello c'est le proccess Antoine qui vous parle \n");
        
        int quantity;
        float price;
        requestQuantityAndPrice(Antoine, sw1, fd,"plein", 20,  &quantity, &price);
        requestPayment(Antoine,sw2,fd,"plein",quantity,"5412751234123456",274);


        closePipes(Antoine,fd);

    }

    if(isProcess(Francoise, pid)){ 

        requestAvailableArea(Francoise,sw1,fd,"creux");
        
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
    int min = N*id;
    int max = N*(id + 1);
    for(int i = 0; i<min; i++)
        close(fd[i][1]);
    for(int i = max; i<N*N;i++)
        close(fd[i][1]);
    for(int i = min; i< max;i++)
        close(fd[i][0]);
    

    int rmin = min + (N)*N;
    int rmax = max + (N)*N;
    for(int i = N*N; i<rmin; i++)
        close(fd[i][1]);
    for(int i = rmax; i<2*N*N;i++)
        close(fd[i][1]);
    for(int i = rmin; i< rmax;i++)
        close(fd[i][0]);
}

void closePipes(Process id, int fd[2*N*N][2]){
    int min = N*id;
    int max = N*(id + 1);
    for(int i = 0; i<min; i++)
        close(fd[i][0]);
    for(int i = max; i<N*N;i++)
        close(fd[i][0]);
    for(int i = min; i< max;i++)
        close(fd[i][1]);
    
    int rmin = min + (N)*N;
    int rmax = max + (N)*N;
    for(int i = N*N; i<rmin; i++)
        close(fd[i][0]);
    for(int i = rmax; i<2*N*N;i++)
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

// ETAPE 2

void detectQuery(int fd[2*N*N][2],Process from, Process to){
    char str[40];
    readString(fd[findAdress(from,to)],str);
    processRequest(str,fd,from,to);
}

int findAdress(Process from, Process to){
    if(from<to)
        return 6*from+to;
    else
        return N*N+6*from+to;
}

float requestAvailableArea(Process from, Process to,int fd[N*N][2],char type[]){
    char str[20] = "1,";
    strcat(str,type);
    writeString(fd[findAdress(from,to)],str);
    readString(fd[findAdress(to,from)],str);
    fprintf(stderr,"%s, la surface disponible de palette de type %s est de %s m^2\n",getName(from),type,str);
    return atof(str);
}

void requestQuantityAndPrice(Process from, Process to,int fd[N*N][2],char type[], float area, int *quantity,float *price){
    char str1[20] = "2,";
    char str2[20];
    char str3[20];
    sprintf(str1, "2,%s,%.2f", type,area);

    writeString(fd[findAdress(from,to)],str1);
    readString(fd[findAdress(to,from)],str2);
    readString(fd[findAdress(to,from)],str3);
    fprintf(stderr,"%s, si vous voulez une surface de %.2f m^2. Il vous faudra acheter %s palettes de type %s aux prix de %s€\n",getName(from),area,str2,type,str3);
    *quantity = atoi(str2);
    *price = atof(str3);

}

void requestPayment(Process from, Process to,int fd[N*N][2],char type[], int quantity, char cardNumber[16],short crypto){
    char str1[40];
    char str2[20];
    char str3[20];
    sprintf(str1,"3,%s,%d,%s,%hd",type,quantity,cardNumber,crypto);
    writeString(fd[findAdress(from,to)],str1);
    readString(fd[findAdress(to,from)],str2);
    if(strcmp(str2,"1")==0){
        readString(fd[findAdress(to,from)],str3);
        fprintf(stderr,"%s : Succès du paiement de %d palettes de type %s pour %s€.\n",getName(from),quantity,type,str3);
    } else {
        fprintf(stderr,"Echec du paiement de %d palettes de type %s.\n",quantity,type);
    }
}


// ------- Responss of Process -------
void processRequest(char str[], int fd[2*N*N][2], Process from, Process to){
    int index = atoi(strtok (str,","));
    char *content = strtok(NULL,"");
    switch(index){
        case 1:
            respondAvailableArea(content,fd,from,to);
            break; 
        case 2:
            respondPaletSize(content,fd,from,to);
            break;
        case 3:
            respondPayment(content,fd,from,to);
            break;
        default:
            break;
    }
}

char* getName(Process user){
    if(user == 2)
        return "Antoine";
    if(user == 3)
        return "Francoise";
    if(user ==4)
        return "Jule";
    if(user == 5)
        return "Anne";
    return "";
}

void respondAvailableArea(char content[],int fd[2*N*N][2],Process from, Process to){
    char response[20];
    if(strcmp(content,"creux")==0)
        sprintf(response, "%.2f", surfaceDispoCreux);
    else if (strcmp(content,"plein")==0) 
        sprintf(response, "%.2f", surfaceDispoPlein);
    writeString(fd[findAdress(to,from)],response);
}

void respondPaletSize(char content[],int fd[2*N*N][2],Process from, Process to){
    char *type = strtok (content,",");
    char *area = strtok(NULL,"");

    char response1[20];
    char response2[20];
    int quantity = SurfacetoQuantity(atof(area));
    float price;
    if(strcmp(content,"creux")==0)
        price = paletteCreuxPrice*quantity;
    if(strcmp(content,"plein")==0)
        price = palettePleinPrice*quantity;
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

void respondPayment(char content[],int fd[2*N*N][2],Process from, Process to){
    char *type = strtok (content,",");
    int quantity = atoi(strtok(NULL,","));
    char *cardNumber = strtok(NULL,",");
    char *crypto = strtok(NULL,"");

    float price;
    if(strcmp(type,"creux")==0)
        price = paletteCreuxPrice*quantity;
    if(strcmp(type,"plein")==0)
        price = palettePleinPrice*quantity;
    
    char strprice[20];
    sprintf(strprice, "%.2f", price);

    if((strcmp(cardNumber,cardNumberRegistry[from-2])==0) && (strcmp(crypto,cardCryptoRegistry[from-2])==0)){
        writeString(fd[findAdress(to,from)],"1");
        writeString(fd[findAdress(to,from)],strprice);
    }

}