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

 int sock;
    sock = socket(res->ai_family, res->ai_socktype,0);
    if (sock<0) {
        perror("Erreur lors de la création du socket");
        return 1;
    }
    
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

// Envoyer la requête au serveur
    if (sendto(sock, rrq, rrq_len, 0, res->ai_addr, res->ai_addrlen) < 0) {
        perror("Erreur lors de l'envoi de la requête"); // Afficher une erreur si l'envoi échoue
        free(rrq);            // Libérer la mémoire allouée pour la requête
        close(sock);        // Fermer le socket
        freeaddrinfo(res);    // Libérer les ressources de getaddrinfo
        return 1;             // Arrêter le programme avec une erreur
    }
    printf("Requête RRQ envoyée pour le fichier '%s'.\n", filename); // Confirmation d'envoi

    // Réception et traitement des données
    FILE *outfile = fopen(filename, "wb"); // Créer un fichier local pour stocker les données
    if (!outfile) {
        perror("Erreur lors de la création du fichier local");
        free(rrq);
        close(sock);
        return 1;
    }

    char buffer[516]; // Taille maximale d'un paquet TFTP (512 octets de données + en-tête)
    ssize_t bytes_received;
    int block_number = 1;
    while ((bytes_received = recvfrom(sock, buffer, sizeof(buffer), 0, NULL, NULL)) > 0) {
        // Vérification de l'opcode (0x03 pour DATA)
        if (buffer[1] != 0x03) {
            fprintf(stderr, "Paquet inattendu reçu (opcode: %d).\n", buffer[1]);
            break;
        }

        // Extraire le numéro de bloc
        int received_block = (buffer[2] << 8) | buffer[3];
        if (received_block != block_number) {
            fprintf(stderr, "Numéro de bloc inattendu : %d (attendu : %d).\n", received_block, block_number);
            break;
        }

        // Écrire les données dans le fichier (à partir du 4ème octet)
        fwrite(buffer + 4, 1, bytes_received - 4, outfile);
}    