//
//Created by Benedetta Bonaccorso on 20/10/24.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "game.h"

#define PORT 12345 // Porta del server
#define SERVER_IP "127.0.0.1" // Indirizzo IP del server

typedef struct {
    char type; // 'B' = ball update, 'P' = paddle update
    int x, y;  /* x: Identifica il giocatore (0 o 1) in caso di aggiornamento racchetta.
                  y: La posizione della racchetta o della pallina.*/
} Packet;
// griglia
void display_game(GameState *state, int player_id) {
    system("clear");
    printf("Score: Player 1 - %d | Player 2 - %d\n", state->score_left, state->score_right);

    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (x == state->ball_x && y == state->ball_y)
                printf("O "); // Mostra la pallina
            else if (x == 0 && y >= state->paddle_left && y < state->paddle_left + PADDLE_SIZE)
                printf("| "); //Mostra la racchetta del giocatore 0
            else if (x == GRID_SIZE - 1 && y >= state->paddle_right && y < state->paddle_right + PADDLE_SIZE)
                printf("| ");  // Mostra la racchetta del giocatore 1
            else
                printf(". ");  // Celle vuote
        }
        printf("\n");
    }
}

void client_loop(int player_id) {
    int sockfd;
    struct sockaddr_in server_addr; //struct per l'indirizzo del server
    Packet packet;  // Pacchetto da inviare
    GameState state; // Stato attuale del gioco

    // Configurazione socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); //socket udp
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;  // IPv4
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP); // Indirizzo del server
    server_addr.sin_port = htons(PORT); // Porta del server

    packet.type = 'P';  // Tipo di pacchetto: aggiornamento racchetta
    packet.x = player_id;  // ID del giocatore
    packet.y = 0; // Posizione iniziale della racchetta
    sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    // Invia il pacchetto contenente il tipo di aggiornamento ('P' per paddle) e la posizione
    // al server tramite il socket. Specifica l'indirizzo e la porta del server.
    printf("Client %d connesso al server\n", player_id);

    fd_set read_fds; // Set per monitorare input da socket e tastiera
    int max_fd = sockfd > STDIN_FILENO ? sockfd : STDIN_FILENO;
    // Determina il valore più alto tra il file descriptor del socket (sockfd)
    // e lo standard input (STDIN_FILENO). Questo è necessario per il funzionamento
    // di select(), che richiede il file descriptor più alto da monitorare.

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);    // Aggiungi socket per ricezione dati
        FD_SET(STDIN_FILENO, &read_fds); // Aggiungi input da tastiera

        // Aspetta eventi su uno dei file descriptor
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("Errore in select()");
            break;
        }
        // Controlla se ci sono dati dal server
        if (FD_ISSET(sockfd, &read_fds)) {
            ssize_t recv_len = recvfrom(sockfd, &state, sizeof(state), 0, NULL, NULL);
            if (recv_len > 0) {
                display_game(&state,player_id); // Visualizza lo stato ricevuto
            } else {
                perror("Errore nella ricezione dal server");
                break;
            }
        }

        // Controlla input utente
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            char buffer[16]; // Buffer per l'input
            if (fgets(buffer, sizeof(buffer), stdin)) {
            // Controlla se l'utente vuole uscire
            if (strncmp(buffer, "exit", 4) == 0) {
                    printf("Uscita dal gioco.\n");
                    break; // Esce dal loop principale
                }



                packet.y = atoi(buffer); // Converte l'input in intero

                // Validazione input
                if (packet.y < 0 || packet.y > GRID_SIZE - PADDLE_SIZE) {
                    printf("Input non valido. Riprova.\n");
                    continue;
                }

                packet.type = 'P';
                packet.x = player_id;

                // Invia aggiornamento al server
                sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
            } else {
                perror("Errore nella lettura dell'input");
            }
        }
    }

    close(sockfd);
}

int main(int argc, char *argv[]) {
    // Verifica che venga passato l'ID del giocatore (player_id).
    if (argc < 2) {
        // Stampa un messaggio di utilizzo per informare l'utente del formato corretto.
        printf("Usage: %s <player_id>\n", argv[0]);
        return 1;
    }

    int player_id = atoi(argv[1]);
    if (player_id != 0 && player_id != 1) {//bisogna inserire player 0 o player 1
        printf("Errore: player_id deve essere 0 o 1.\n");
        return 1;
    }

    client_loop(player_id);
    return 0;
}
