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

int main () {
  printf("Bonjour, bienvenue au service de vente de palettes de parpaings!\n");
  fflush(stdout);
  sleep(2);
  printf("$$Connexion au serveur en cours... veuillez patienter$$\n");
  fflush(stdout);
  sleep(3);
  printf("$$Connexion au serveur %s reussie$$\n", scenario[0]);
  fflush(stdout);

  int pipeServ_Ach[2];
  char buf;
  char* msg = "Veuillez saisir votre nom: ";

  if(pipe(pipeServ_Ach) < 0)
    report_and_exit("pipeServ_Ach");
  pid_t cpid = fork();
  if(cpid < 0) 
    report_and_exit("fork");

  if(0 == cpid) { /* child  */
      close(pipeServ_Ach[WriteEnd]);
      while(read(pipeServ_Ach[ReadEnd], &buf, 1) > 0) 
        write(STDOUT_FILENO, &buf, sizeof(buf));
      close(pipeServ_Ach[ReadEnd]);
  } else { /*parent*/
      close(pipeServ_Ach[ReadEnd]);
      write(pipeServ_Ach[WriteEnd], msg, strlen(msg));
      close(pipeServ_Ach[WriteEnd]);
      wait(NULL);
  }
  return 0;
}
