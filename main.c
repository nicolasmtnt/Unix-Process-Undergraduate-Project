// Illustration pour mieux comprendre le code à cette adresse : https://github.com/nicolasmtnt/Unix-Process-Undergraduate-Project/blob/main/README.md

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

//Veuillez remplacer les valeurs pour le scénario de votre choix
#define N 6 // nombre de process ( 2 clients + 2 serveurs + 2 livreurs)
#define TYPE "plein" // valeurs possibles : "plein" , "creux"
#define AREA 40 // valeurs possibles :  float positif
#define NOMBRE_PALETTES 20 //valeurs possibles : entier positifs
#define CLIENT Antoine // valeurs possibles : Antoine,  Francoise
#define SERVEUR sw1 // valeurs possibles : sw1 , sw2
#define LIVREUR Jule // valeurs possibles : Jule , Anne

float PaletteSize = 50*0.2*0.2;
float palettePleinPrice = 5.99;
float paletteCreuxPrice = 4.99;

typedef enum { sw1, sw2, Antoine, Francoise, Jule, Anne } Process;
typedef enum { false,true } bool;

char cardNumberRegistry[2][20] = {"5412751234123456","4000123456789010" };// Numero de carte bleu de Antoine et de Francoise;
char cardCryptoRegistry[2][20] = {"274","947"}; // crytpogramme de la carte bleu de Antoine et de Francoise
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
void requestDelivery(Process from, Process to, int fd[N*N][2], char type[], int quantity,Process client);
void requestSignature(Process from, Process to, int fd[N*N][2], char type[], int quantity);

void processRequest(char str[],int fd[2*N*N][2],Process from, Process to);
char* getName(Process user);
void respondAvailableArea(char content[],int fd[2*N*N][2],Process from, Process to);
void respondPaletSize(char content[],int fd[2*N*N][2],Process from, Process to);
int SurfacetoQuantity(float surface);
void respondPayment(char content[],int fd[2*N*N][2],Process from, Process to);
void respondDelivery(char content[],int fd[2*N*N][2],Process from, Process to);
void respondSignature(char content[],int fd[2*N*N][2],Process from, Process to);

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

    if(isProcess(sw1, pid) && SERVEUR == sw1){ 
        setPipes(sw1,fd);
        detectQuery(fd,CLIENT,SERVEUR); //reception de requestQuantityAndPrice() de CLIENT
        detectQuery(fd,CLIENT,SERVEUR); //reception de requestPayment() de CLIENT
        requestDelivery(SERVEUR, LIVREUR, fd, TYPE,NOMBRE_PALETTES, CLIENT); //affectation de la livraison à LIVREUR
        closePipes(sw1,fd);
    }

    if(isProcess(sw2, pid) && SERVEUR == sw2){ 
        setPipes(sw2,fd);
        detectQuery(fd,CLIENT,SERVEUR); //reception de requestQuantityAndPrice() de CLIENT
        detectQuery(fd,CLIENT,SERVEUR); //reception de requestPayment() de CLIENT
        requestDelivery(SERVEUR, LIVREUR, fd, TYPE,NOMBRE_PALETTES, CLIENT); //affectation de la livraison à LIVREUR
        closePipes(sw2,fd);
    }

    if(isProcess(Antoine, pid) && CLIENT == Antoine){ 
        setPipes(Antoine,fd);
        int quantity;
        float price;
        requestQuantityAndPrice(CLIENT, SERVEUR, fd,TYPE, AREA,  &quantity, &price); // CLIENT demande a SERVEUR quel est le prix et la quantité pour une certaine surface
        requestPayment(CLIENT,SERVEUR,fd,TYPE,quantity,"5412751234123456",274); //Achat
        detectQuery(fd,LIVREUR,CLIENT); // reception de la livraison
        closePipes(CLIENT,fd);
    }

    if(isProcess(Francoise, pid) && CLIENT == Francoise){ 
        setPipes(Francoise,fd);
        int quantity;
        float price;
        requestQuantityAndPrice(CLIENT, SERVEUR, fd,TYPE, AREA,  &quantity, &price); // CLIENT demande a SERVEUR quel est le prix et la quantité pour une certaine surface
        requestPayment(CLIENT,SERVEUR,fd,TYPE,quantity,"5412751234123456",274); //Achat
        detectQuery(fd,LIVREUR,CLIENT); // reception de la livraison
        closePipes(Francoise,fd);
    }

    if(isProcess(Jule, pid) && LIVREUR == Jule){ 
        setPipes(Jule,fd);

        detectQuery(fd,SERVEUR,LIVREUR);
        closePipes(Jule,fd);
    }

    if(isProcess(Anne, pid) && LIVREUR == Anne){ 
        setPipes(Anne,fd);
        detectQuery(fd,SERVEUR,LIVREUR);
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

// ------- Process Request -------

float requestAvailableArea(Process from, Process to,int fd[N*N][2],char type[]){
    fprintf(stderr,"%s : Je demande à %s combien de palettes de type [%s] sont disponibles.\n",getName(from),getName(to),type);
    char str[20] = "1,";
    strcat(str,type);
    fflush(stdout);
    writeString(fd[findAdress(from,to)],str);
    usleep(500000);
    readString(fd[findAdress(to,from)],str);
    fprintf(stderr,"%s : %s me repond que la surface disponible de palette de type [%s] est de %s m^2.\n\n",getName(from),getName(to),type,str);
    fflush(stdout);
    return atof(str);
}

void requestQuantityAndPrice(Process from, Process to,int fd[N*N][2],char type[], float area, int *quantity,float *price){
    fprintf(stderr,"%s : Je demande à %s combien de palettes de type [%s] ai-je besoin pour une surface de %.2f m^2 et combien cela me coutera\n",getName(from),getName(to),type,area);
    char str1[20] = "2,";
    char str2[20];
    char str3[20];
    sprintf(str1, "2,%s,%.2f", type,area);
    writeString(fd[findAdress(from,to)],str1);
    usleep(500000);
    readString(fd[findAdress(to,from)],str2);
    readString(fd[findAdress(to,from)],str3);
    fprintf(stderr,"%s : %s me repond qu'il me faudra acheter %s palettes aux prix de %s€\n\n",getName(from),getName(to),str2,str3);
    fflush(stdout);
    *quantity = atoi(str2);
    *price = atof(str3);
}

void requestPayment(Process from, Process to,int fd[N*N][2],char type[], int quantity, char cardNumber[16],short crypto){
    fprintf(stderr,"%s : J'achète (par le serveur %s) %d palettes de type [%s].\n - Numero Carte de Paiement : %s\n - Cryptogramme : %d\n",getName(from),getName(to),quantity,type,cardNumber,crypto);
    fflush(stdout);
    char str1[40];
    char str2[20];
    char str3[20];
    sprintf(str1,"3,%s,%d,%s,%hd",type,quantity,cardNumber,crypto);
    writeString(fd[findAdress(from,to)],str1);
    usleep(500000);
    readString(fd[findAdress(to,from)],str2);
    if(strcmp(str2,"1")==0){
        readString(fd[findAdress(to,from)],str3);
        fprintf(stderr,"%s : Succès du paiement de %d palettes de type [%s] pour %s€.\n\n",getName(from),quantity,type,str3);
        fflush(stdout);
    } else {
        fprintf(stderr,"Echec du paiement de %d palettes de type [%s].\n\n",quantity,type);
    }
}

void requestDelivery(Process from, Process to, int fd[N*N][2], char type[], int quantity,Process client){
    usleep(500000);
    fprintf(stderr,"%s : Je demande à %s de livrer un colis de %d palettes de type [%s] à %s.\n",getName(from),getName(to),quantity,type,getName(client));
    fflush(stdout);
    char request[20];
    char response[20];
    sprintf(request,"4,%d,%s,%d",client,type,quantity);
    writeString(fd[findAdress(from,to)],request);
}

void requestSignature(Process from, Process to, int fd[N*N][2], char type[], int quantity){
    sleep(1);
    fprintf(stderr,"%s : Je demande à %s de signer un des deux bons de livraison.\n",getName(from),getName(to));
    fflush(stdout);
    char deliveryNote1[20];
    sprintf(deliveryNote1,"5,%s,%d,unsigned",type,quantity);
    char deliveryNote2[20];
    strcpy(deliveryNote2,deliveryNote1);
    writeString(fd[findAdress(from,to)],deliveryNote2);
    usleep(50000);
    readString(fd[findAdress(to,from)],deliveryNote2);
    fprintf(stderr,"%s : %s m'a rendu le bon de livraison signé.\n\n",getName(from),getName(to));
    fflush(stdout);
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
        case 4:{
            respondDelivery(content,fd,from,to);}
            break;
        case 5:
            respondSignature(content,fd,from,to);
            break;
        default:
            break;
    }
}

char* getName(Process user){
    if(user == 0)
        return "sw1";
    if(user == 1)
        return "sw2";
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

void respondDelivery(char content[],int fd[2*N*N][2],Process from, Process to){
    Process client = atoi(strtok(content,","));
    char *type = strtok(NULL,",");
    int quantity = atoi(strtok(NULL,","));
    requestSignature(to,client,fd,type,quantity);
}

void respondSignature(char content[],int fd[2*N*N][2],Process from, Process to){
    char *type = strtok(content,",");
    int quantity = atoi(strtok(NULL,","));
    char signedDeliveryNote[20];
    sprintf(signedDeliveryNote,"5,%s,%d,signed",type,quantity);
    writeString(fd[findAdress(to,from)],signedDeliveryNote);
}
