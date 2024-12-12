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