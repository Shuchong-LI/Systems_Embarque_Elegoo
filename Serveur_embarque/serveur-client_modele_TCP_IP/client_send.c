#include <stdio.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

/* Rôle du client : envoyer une demande de connexion à un serveur,
   envoyer une chaîne de caractères à ce serveur (la chaîne de
   caractère est à saisir au clavier), recevoir un entier et comparer
   cet entier avec le nombre d'octets envoyés. L'idée est de mettre en
   place les bases qui vous permettront par la suite une mise en
   oeuvre correcte des communications en TCP et de vérifier les
   échanges effectués */

int main(int argc, char *argv[]) {

  if (argc != 3){
    printf("utilisation : %s ip_serveur port_serveur\n", argv[0]);
    exit(0);
  }

  /* Etape 1 : créer une socket */   
  int dsClient = socket(PF_INET, SOCK_STREAM, 0);

  if (dsClient == -1){
    printf("Client : pb creation socket\n");
    exit(1); 
  }
  printf("Client: creation de la socket : ok\n");
  
  /* Etape 2 : designer la socket du serveur : avoir une structure qui
     contient l'adresse de cette socket (IP + numéro de port. */
  printf("client:   ip:%s   port:%s\n",argv[1],argv[2]);
  struct sockaddr_in adrServ;
  adrServ.sin_addr.s_addr = inet_addr(argv[1]);
  adrServ.sin_family = PF_INET;
  adrServ.sin_port = htons(atoi(argv[2]));
  int lgAdr = sizeof(adrServ);
    
  /* Etape 3 : envoyer une demande de connexion au serveur.*/
  
  int conn = connect(dsClient, (struct sockaddr*)&adrServ, lgAdr);
  
  if (conn < 0){
    perror ("Client: pb au connect :");
    close (dsClient); 
    exit (1); 
  }

  // je continue à alimenter le programme avec traces d'exécution
  printf("Client : demande de connexion reussie \n");

  // Je peux tester l'exécution de cette étape avant de passer à la suite.
  
  /* Etape 4 : envoyer un message au serveur. Ce message est une chaîne de caractères saisie au clavier. Vous pouvez utiliser une autre fonction pour la saisie. */

  printf("saisir un message à envoyer (moins de 200 caracteres) \n");
  char m[202]; 
  fgets(m, sizeof(m), stdin); // copie dans m la chaîne saisie que
			      // clavier (incluant les esaces et le
			      // saut de ligne à la fin).
  m[strlen(m)-1]  = '\0'; // je retire le saut de ligne                                      

  printf("message: %s    size: %d\n",m, strlen(m));
  int nbOctetEmis = send(dsClient,m,strlen(m),0);
  /* Traiter TOUTES les valeurs de retour (voir le cours ou la documentation). */
  if(nbOctetEmis < 0)
  {
    perror("Client: pd d'envoi :");
    close(dsClient);
    exit(1);
  }

  close (dsClient);
  printf("Client : je termine\n");
}
