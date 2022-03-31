/******************************************************************************
Essai de projet SE.
*******************************************************************************/
#include <sys/wait.h>		/* wait */
#include <stdio.h>
#include <stdlib.h>		/* exit functions */
#include <unistd.h>		/* read, write, pipe, _exit */
#include <string.h>

#define ReadEnd  0
#define WriteEnd 1
#define SIZE 7
/*Definition du scenario*/
/*Definir comme suit dans cet ordre: serveur web (sw1, sw2), acheteur (Antoine,
Francoise), transporteur(Jule, Anne), article (creux, plein), prix(nombre entier)
quantite en m2 de stock (entier), surface en m2 (entier)*/
char scenario[SIZE][25] = { "sw1", "Antoine", "Jule", "creux", "25", "500", "250" };

void report_and_exit (const char *msg) {
  perror(msg);
  exit (-1);   /** failure **/
}

int
main ()
{
printf("Bonjour %s, bienvenue au service de vente de palettes de parpaings!\n", scenario[1]);
sleep(2);
printf("$$Connexion au serveur en cours... veuillez patienter$$\n");
sleep(3);
printf("$$Connexion au serveur %s reussie$$\n", scenario[0]);

int pipeServ_Ach[2];
int pipeAch_Serv[2];
char buf;

if(pipe(pipeAch_Serv) < 0 ) report_and_exit("pipeAch_Serv");
if(pipe(pipeServ_Ach) < 0) report_and_exit("pipeServ_Ach");
pid_t cpid_1 = fork();
if(cpid_1< 0) report_and_exit("fork");

if(0 == cpid_1) { /*child*/
    char concat_str[100];
    close(pipeServ_Ach[WriteEnd]);
    read(pipeServ_Ach[ReadEnd], concat_str, 100);
    printf("%s", concat_str);
    close(pipeServ_Ach[ReadEnd]);
    close(pipeAch_Serv[ReadEnd]);
    write(pipeAch_Serv[WriteEnd], scenario[3], strlen(scenario[3]) + 1);
    close(pipeAch_Serv[WriteEnd]);
} else { /*parent*/
    char concat_str[100];
    char* msg_1 = "Veuillez selectionner un type de parpaing : ";
    close(pipeServ_Ach[ReadEnd]);
    write(pipeServ_Ach[WriteEnd], msg_1, strlen(msg_1) + 1);
    close(pipeServ_Ach[WriteEnd]);
    wait(NULL);
    close(pipeAch_Serv[WriteEnd]);
    read(pipeAch_Serv[ReadEnd], concat_str, 100);
    sleep(3);
    printf("%s \n", concat_str);
    close(pipeAch_Serv[ReadEnd]);
}

return(0);
}
