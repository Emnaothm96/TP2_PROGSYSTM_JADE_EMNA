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
    char *filename = argv[2]; // Nom du fichier à téléverser
    char *port = "69"; // Port TFTP (par défaut : 69)

    struct addrinfo hints, *res;
    int sockfd;

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
 int sock;
    // Création du socket
    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock <0 ) {
        perror("Erreur lors de la création du socket");
        return 1;
    }

    // Construction de la requête WRQ
    char mode[] = "octet"; // Mode de transfert
    size_t wrq_len = 2 + strlen(filename) + 1 + strlen(mode) + 1;
    char *wrq = malloc(wrq_len);
    if (!wrq) {
        perror("Erreur d'allocation mémoire");
        close(sock);
        return 1;
    }
    memset(wrq, 0, wrq_len);
    wrq[0] = 0x00; // Opcode pour WRQ
    wrq[1] = 0x02; // Opcode pour écriture
    strcpy(wrq + 2, filename); // Nom du fichier
    strcpy(wrq + 2 + strlen(filename) + 1, mode); // Mode de transfert

struct sockaddr * serv_adrr = res->ai_addr;
socklen_t serv_adrr_len = res->ai_addrlen;

    // Envoi de la requête WRQ au serveur
    if (sendto(sock, wrq, wrq_len, 0, serv_adrr,serv_adrr_len) < 0 ) {
        perror("Erreur lors de l'envoi de la requête WRQ");
        free(wrq);
        close(sock);
        return 1;
    }
    printf("Requête WRQ envoyée pour le fichier '%s'.\n", filename);
}
