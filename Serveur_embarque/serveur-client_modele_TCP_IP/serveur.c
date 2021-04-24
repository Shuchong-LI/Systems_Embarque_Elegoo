#include <stdio.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
  
  /* Etape 1 : créer une socket (la socket qui permettra de recevoir
     les demandes de connexion.*/
 
  int dsServeur = socket(AF_INET, SOCK_STREAM, 0);
  
  if (dsServeur == -1){
    perror("Serveur : probleme creation socket");
    exit(1); 
  }
  printf("Serveur: creation de la socket : ok\n");

  int PORT = atoi(argv[1]);
  
  /* Etape 2 : nommer la socket. Elle aura une ou des IP de la machine
     sur laquelle le programme sera exécuté et le numéro de port passé
     en paramètre*/

  printf("PORT: %d \n",PORT);
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(PORT);

  if(bind(dsServeur, (struct sockaddr*)&server, sizeof(server)) < 0){
    perror("Serveur : erreur bind");
    close(dsServeur); 
    exit(1);
  }

  printf("Serveur: nommage : ok\n");


  /* Etape 3 : mise en ecoute de la socket nommée. Cette opération
     dédie la socket à la réception des demandes de connexion. Pour
     cet exercice, limiter la file des demandes de connexions à 5. */
  
  int ecoute = listen(dsServeur, 5);
  if (ecoute < 0){
    printf("Serveur : je suis sourd(e)\n");
    close (dsServeur);
    exit (1);
  } 
 
  printf("Serveur: mise en écoute : ok\n");
 
  /* etape 4 : attendre et traiter une demande de connexion d'un
     client. Rappel : lorsque le serveur accepte une demande de
     connexion, il crée une nouvelle socket. Cette dernière est
     connectée à celle du client et elle est à utiliser pour
     communiquer avec lui.*/

  printf("Serveur : j'attends la demande du client 'Ecran' \n"); 
  
  struct sockaddr_in adClient_ecran ; // pour obtenir l'adresse du client accepté.
  socklen_t lgClient = sizeof(struct sockaddr_in);

  int dsClient_ecran = accept(dsServeur, (struct sockaddr*)&adClient_ecran, &lgClient);
  if (dsClient_ecran < 0)
  { 
    perror ( "Serveur, probleme accept :");
    close(dsServeur);
    exit (1);
  }
  printf("Serveur: le client %s : %d est connecté  \n", inet_ntoa(adClient_ecran.sin_addr),  ntohs(adClient_ecran.sin_port));
  
  printf("Serveur : j'attends la demande d'un client\n"); 
  
  struct sockaddr_in adClient ; // pour obtenir l'adresse du client accepté.
  lgClient = sizeof(struct sockaddr_in);

  char buffer[1000];
  int nbOctetsRecus;
  int nbOctetsEmis;
  int dsClient;
  while(1)
  {
    dsClient = accept(dsServeur, (struct sockaddr*)&adClient, &lgClient);
    if (dsClient < 0)
    { 
      perror ( "Serveur, probleme accept :");
      close(dsServeur);
      exit (1);
    }
    printf("Serveur: le client %s : %d est connecté  \n", inet_ntoa(adClient.sin_addr),  ntohs(adClient.sin_port));
    
    /* Etape 5 : réception d'un message de type chaîne de caractères */
 
    nbOctetsRecus = recv (dsClient, buffer, sizeof(buffer), 0);

    if(nbOctetsRecus < 0)
    {
      perror("Serveur: pb reception:");
      close(dsClient);
      exit(1);
    }
    buffer[nbOctetsRecus]='\0';
    printf("Serveur : j'ai recu %d octets \n", nbOctetsRecus);
    printf("Serveur : contenu du message : %s\n", buffer);
  
    nbOctetsEmis = send(dsClient_ecran, buffer, nbOctetsRecus, 0);

    printf("nbOctetsEmis: %d\n",nbOctetsEmis);
    if(nbOctetsEmis < 0)
    {
      perror("Serveur: pd d'envoi :");
      close(dsClient);
      exit(1);
    }
    close (dsClient);
  }
    
  /* Etape 7 : fermeture de la socket du client */ 
  printf("Serveur : fin du dialogue avec le client\n");
  close (dsClient_ecran);
  
  // Etape 8 : pour cet exercice, je ne traite qu'un client, donc, je termine proprement.
  close (dsServeur);
  printf("Serveur : je termine\n");
}








