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
    char *port = "1069"; // Port TFTP

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
}    