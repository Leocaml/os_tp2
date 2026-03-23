#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 9998
#define LOCALHOST "127.0.0.1"

int main(int argc, char* argv[]) {
    int sid;
    struct sockaddr_in servAddr, from;
    socklen_t flen = sizeof(from);
    char buffer[1024];
    char buffer_ar[1024];
    int msg_len = 0;
    
    if (argc < 2) {
        fprintf(stderr, "Utilisation : %s <code_beuip> [args...]\n", argv[0]);
        exit(1);
    }

    if ((sid = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(2);
    }

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = inet_addr(LOCALHOST);

    char code = argv[1][0];

    switch (code) {
        case '3':
            strcpy(buffer, "3BEUIP");
            msg_len = 6;
            break;
        case '4':
            if (argc != 4) { fprintf(stderr, "Manque pseudo ou message\n"); exit(3); }
            msg_len = sprintf(buffer, "4BEUIP%s", argv[2]);
            msg_len++; // caractère nul de séparation
            strcpy(buffer + msg_len, argv[3]);
            msg_len += strlen(argv[3]);
            break;
        case '5':
            if (argc != 3) { fprintf(stderr, "Message manquant\n"); exit(3); }
            msg_len = sprintf(buffer, "5BEUIP%s", argv[2]);
            break;
        case '0':
            strcpy(buffer, "0BEUIP");
            msg_len = 6;
            break;
        default:
            fprintf(stderr, "Code inconnu (0,3,4,5)\n");
            exit(4);
    }

    // Envoi au serveur local
    if (sendto(sid, buffer, msg_len, 0, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("sendto");
        exit(1);
    }

    // --- [AJOUT ÉTAPE 1] : Attente de l'Accusé de Réception (AR) ---
    printf("Message envoyé. Attente de l'AR...\n");
    int n = recvfrom(sid, buffer_ar, sizeof(buffer_ar) - 1, 0, (struct sockaddr *)&from, &flen);
    if (n > 0) {
        buffer_ar[n] = '\0';
        printf("Serveur dit : %s\n", buffer_ar);
    }

    close(sid);
    return 0;
}