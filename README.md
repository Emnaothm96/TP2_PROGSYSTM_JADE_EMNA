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
sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock== -1) {
        perror("Erreur lors de la création du socket");
        return 1;
    }
```
TP2.c    