# TP2_PROGSYSTM_JADE_EMNA

## Explication du TP: 
Dans ce TP, on va créer un programme qui permet d'envoyer et de recevoir des fichiers avec un serveur en utilisant un protocole simple appelé TFTP (Trivial File Transfer Protocol). Ce protocole est souvent utilisé pour des choses comme installer des systèmes d'exploitation sur des ordinateurs via le réseau ou mettre à jour des équipements comme des routeurs. On doit faire deux commandes : une pour télécharger un fichier depuis le serveur (gettftp <host> <file>) et une autre pour envoyer un fichier au serveur (puttftp <host> <file>). On va apprendre à envoyer les bonnes requêtes au serveur, recevoir les réponses, et tester tout ça avec Wireshark pour voir ce qui se passe sur le réseau. 

## Question 1 : 

Dans cette partie du code, on configure les arguments que le programme doit recevoir pour fonctionner.On doit fournir deux informations : le nom du serveur (ou son adresse, comme 127.0.0.1 pour localhost) et le nom du fichier qu’on veut télécharger. Si ces informations ne sont pas données, le programme affiche un message d’erreur expliquant comment l'utiliser . On stocke ensuite ces informations dans des variables pour les utiliser dans le reste du programme : domain contient l'adresse du serveur, filename contient le nom du fichier à télécharger, et port est fixé à 69, qui est le port par défaut utilisé par le protocole TFTP

```c title="TP2.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <domain> <file>\n", argv[0]);
        return 1;
    }

    char *domain = argv[1]; // Adresse du serveur (ex: "127.0.0.1")
    char *filename = argv[2]; // Nom du fichier à télécharger
    char *port = "69"; // Port TFTP
```
TP2.c

## Question 2 : 

Dans cette partie du programme, on configure les informations nécessaires pour se connecter au serveur TFTP. On commence par initialiser une structure (hints) pour préciser qu’on veut utiliser une adresse IPv4 (AF_INET) et le protocole UDP (SOCK_DGRAM), car c’est ce qu’utilise TFTP. Ensuite, on utilise la fonction getaddrinfo pour convertir le nom du serveur (ou son adresse) et le numéro de port en une structure contenant tous les détails nécessaires pour communiquer avec lui. Si cette étape échoue, le programme affiche une erreur et s’arrête

```c title="TP2.c"
struct addrinfo hints, *res, *p;

    // Initialisation de hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP

    // Résolution de l'adresse du serveur
    int status = getaddrinfo(domain, port, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "Erreur dans getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

 ```
 TP2.c   

 ## Question 3 : 

Dans cette partie du programme on crée la fonction socket,c'est essentielle pour établir une communication entre notre programme (le client TFTP dans ce cas) et un serveur TFTP. Le socket permet d'envoyer des requêtes au serveur et de recevoir des réponses.On utilise les paramètres définis précédemment (comme AF_INET pour indiquer qu’on communique en IPv4 et SOCK_DGRAM pour spécifier qu’on utilise UDP) pour configurer correctement cette connexion. Une fois le socket créé, il nous donne un identifiant (sock) que le programme utilisera pour toutes les communications avec le serveur.

```c title="TP2.c"
int sock;
    sock = socket(res->ai_family, res->ai_socktype,0);
    if (sock<0) {
        perror("Erreur lors de la création du socket");
        return 1;
    }
```
TP2.c    

## Question 4 
###  4)a) 
Dans cette partie du code, on construit une requête TFTP de lecture appelée RRQ (Read Request). Cette requête est le message que le client envoie au serveur pour demander un fichier le mode de transfert est défini par octect, qui permet de transférer kes données en binaire. on calcule d'abord la taille de la requête en additionnant les éléments nécessaires  : 2 octects pour l'opcode, la longueure du nom du fichier, un charactère nul pour terminer ce nom, la longueure du mode de transfert, et un charactère nul pour terminer le mode. ensuite on reserve un espace mémoire pour stocker cette réserve avec malloc. si cela échoue, une erreur est affichée et le programme s'arrête.Ensuite , la mémoire allouée pour la requête est initialisée à zéro avec memset pour éviter toute donnée indésirable. Ensuite, la requête est construite en suivant le format du protocole TFTP : les deux premiers octets contiennent l’opcode 0x01, qui indique une requête de lecture. Les octets suivants contiennent le nom du fichier demandé (filename), suivi d’un caractère nul (\0). Enfin, on ajoute le mode de transfert (mode, ici "octet") également terminé par un caractère nul(comme précedament expliqué).Une fois la requête formée, elle est envoyée au serveur via la fonction sendto, qui utilise le socket UDP précédemment créé. Si l’envoi échoue, un message d’erreur est affiché, la mémoire est libérée, et le programme s’arrête. Si l’envoi réussit, un message confirme que la requête RRQ a bien été envoyée avec le nom du fichier demandé

```c title="TP2.c"

  // Construction de la requête RRQ
    char mode[] = "octet"; // Mode de transfert (octet pour TFTP)
    size_t rrq_len = 2 + strlen(filename) + 1 + strlen(mode) + 1;
    char *rrq = malloc(rrq_len);
    if (!rrq) {
        perror("Erreur d'allocation mémoire");
        return 1;
    }
    memset(rrq, 0, rrq_len);           // Initialiser la mémoire de la requête à zéro
    rrq[0] = 0x00;                     
    rrq[1] = 0x01;                     
    strcpy(rrq + 2, filename);         // Ajouter le nom du fichier demandé
    strcpy(rrq + 2 + strlen(filename) + 1, mode); // Ajouter le mode de transfert ("octet")
struct sockaddr * serv_adrr = res->ai_addr;
socklen_t serv_adrr_len = res->ai_addrlen;
// Envoyer la requête au serveur
    if (sendto(sock, rrq, rrq_len, 0, serv_adrr, serv_adrr_len) < 0) {
        perror("Erreur lors de l'envoi de la requête"); // Afficher une erreur si l'envoi échoue
        free(rrq);            // Libérer la mémoire allouée pour la requête
        close(sock);        // Fermer le socket
        freeaddrinfo(res);    // Libérer les ressources de getaddrinfo
        return 1;             // Arrêter le programme avec une erreur
    }
    printf("Requête RRQ envoyée pour le fichier '%s'.\n", filename); // Confirmation d'envoi

   ``` 
   TP2.c

   ### 4)b)

 Dans cette partie du code, le traitement des paquets de données (DAT) et des acquittements (ACK) assure un transfert fiable entre le client et le serveur, en respectant le protocole TFTP. Tout d’abord, le client reçoit un paquet envoyé par le serveur via la fonction recvfrom. Ce paquet peut contenir jusqu’à 512 octets : 4 octets pour l’en-tête (opcode et numéro de bloc) et jusqu’à 512 octets pour les données utiles du fichier. Après réception, le client vérifie que l’opcode du paquet est bien 0x03, ce qui identifie un paquet de données (DAT). Si l’opcode est incorrect, cela signifie que le paquet reçu est invalide ou inattendu, et le programme interrompt le traitement.

Une fois validé, le client extrait les données du paquet, qui commencent au 4ᵉ octet (buffer + 4), et les écrit dans le fichier local avec fwrite. À chaque paquet reçu, le fichier est progressivement construit, bloc par bloc. Ensuite, le client envoie un acquittement (ACK) au serveur pour confirmer que le bloc de données a bien été reçu. Cet ACK est un message de 4 octets : les deux premiers octets contiennent l’opcode 0x04 pour signaler un acquittement, et les deux derniers octets contiennent le numéro de bloc correspondant au paquet reçu, extrait directement de l’en-tête du paquet. L’ACK est envoyé au serveur via la fonction sendto.

   ```c title="TP2.c"

   // Réception et traitement des données
    FILE *outfile = fopen(filename, "wb"); // Créer un fichier local pour stocker les données
    if (!outfile) {
        perror("Erreur lors de la création du fichier local");
        free(rrq);
        close(sock);
        return 1;
    }

    char buffer[516]; // Taille maximale d'un paquet TFTP (512 octets + en-tête)
    ssize_t bytes_received = recvfrom(sock, buffer, sizeof(buffer), 0, serv_adrr, &serv_adrr_len);
    if (bytes_received == -1) {
        perror("Erreur lors de la réception des données");
        fclose(outfile);
        free(rrq);
        close(sock);
        return 1;
    }
        // Vérification de l'opcode (0x03 pour DATA)
    if (buffer[1] != 0x03) {
        fprintf(stderr, "Paquet inattendu reçu (opcode: %d).\n", buffer[1]);
        fclose(outfile);
        free(rrq);
        close(sock);
        return 1;
    }

    // Écrire les données dans le fichier (à partir du 4ème octet)
    fwrite(buffer + 4, 1, bytes_received - 4, outfile);
    printf("Données reçues et écrites dans le fichier '%s'.\n", filename);

    // Envoi de l'ACK
    char ack[4] = {0x00, 0x04, buffer[2], buffer[3]}; // ACK avec le numéro de bloc
    if (sendto(sock, ack, sizeof(ack), 0, serv_adrr, serv_adrr_len) < 0) {
        perror("Erreur lors de l'envoi de l'ACK");
    } else {
        printf("ACK envoyé pour le bloc 1.\n");
    }

    // Nettoyage
    fclose(outfile);
    free(rrq);
    freeaddrinfo(res);
    close(sock);

    return 0;
  ```
  TP2.c

  Dans cette capture Wireshark, on peut observer que le client envoie une requête de lecture (RRQ) au serveur TFTP pour demander un fichier spécifique (par exemple, ones256 ou alt256). La requête est correctement formatée, comme le montre l'opcode 1 (indiquant une RRQ) et les détails du fichier et du mode de transfert (octet). Cependant, le serveur répond avec un paquet d'erreur (opcode 5), indiquant que le fichier demandé n’a pas été trouvé. Cela signifie que le fichier spécifié dans la requête n’existe pas dans le répertoire où le serveur TFTP cherche les fichiers même si le dossier serveur-tftp est bien placé dans le dossier du TP 
  ![Erreur wireshark](<Capture d’écran du 2024-12-19 10-46-49.png>)

  le problème vient du fait qu'on utilisait le port 69 qui correspond au serveur de l'ensea, hors nous sommes dans cette partie en local. il faut alors se mettre sur le port 1069. de cette façon nous pouviosn envoyer la requete. cependant nous avons eu un deuxième problème concenrant les paramètre dans le recvfrom. en effet les deux derniers paramètres qui permettent de capturer l'adresse source et la longeur de l'adresse du paquet reçu : on avait mis NULL NULL donc on a l'erreur suivante (destination unreachable).

  ![deuxième erreur](<Capture d’écran du 2024-12-19 11-19-29.png>)

  Alors on a posé deux variables premièrement pour ne pas ecraser les valeurs chaque fois et pour les ajouter comme paramètres dans les fonctions recvfrom et sendto 

  ![Les variables ajoutées](<Capture d’écran du 2024-12-19 11-25-16.png>)
  ![Le bon résultat](<Capture d’écran du 2024-12-19 11-26-48.png>)




