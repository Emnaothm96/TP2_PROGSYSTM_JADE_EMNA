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