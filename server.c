#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include "game.h"

#define PORT 12345         // Porta su cui il server ascolta le connessioni
#define BALL_SPEED_MS 500  // Velocità di aggiornamento della pallina in millisecondi
#define MAX_CLIENTS 2      // Numero massimo di client che possono connettersi

typedef struct {
    char type; // Tipo di pacchetto: 'B' = aggiornamento pallina, 'P' = aggiornamento racchetta
    int x, y;  // Posizione della pallina o della racchetta
} Packet;

// Stato di connessione dei client: 0 = non connesso, 1 = connesso
int clients_connected[MAX_CLIENTS] = {0};

// Array per memorizzare gli indirizzi dei client connessi
struct sockaddr_in client_addrs[MAX_CLIENTS];

// Array per memorizzare la lunghezza degli indirizzi dei client connessi
socklen_t client_addr_lens[MAX_CLIENTS];

void server_loop() {
    int sockfd;                           // Socket del server
    struct sockaddr_in server_addr, client_addr; // Indirizzi del server e dei client
    socklen_t addr_len = sizeof(client_addr);    // Lunghezza dell'indirizzo del client
    GameState state;                      // Stato del gioco
    Packet packet;                        // Pacchetto ricevuto dal client

    // Inizializza il gioco con le posizioni iniziali di pallina e racchette
    initialize_game(&state);

    // Seed per la generazione casuale della direzione della pallina
    srand(time(NULL));

    // Creazione del socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Errore nella creazione del socket");
        exit(EXIT_FAILURE);
    }

    // Configurazione dell'indirizzo del server
    server_addr.sin_family = AF_INET;            // Protocollo IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;    // Accetta connessioni da qualsiasi indirizzo
    server_addr.sin_port = htons(PORT);          // Porta specificata

    // Bind del socket all'indirizzo e porta specificati
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Errore nel bind del socket");
        exit(EXIT_FAILURE);
    }

    printf("Server in ascolto sulla porta %d\n", PORT);

    // Loop principale del server
    while (1) {
        // Ricezione di un pacchetto dal client
        ssize_t bytes_received = recvfrom(sockfd, &packet, sizeof(packet), 0,
                                          (struct sockaddr*)&client_addr, &addr_len);
        if (bytes_received < 0) {
            perror("Errore nella ricezione del pacchetto");
            continue; // Continua al prossimo ciclo in caso di errore
        }

        // Determina l'ID del client che ha inviato il pacchetto
        int player_id = packet.x;

        // Controlla se l'ID del client è valido
        if (player_id >= 0 && player_id < MAX_CLIENTS) {
            // Se il client non era ancora connesso, registra il suo indirizzo
            if (!clients_connected[player_id]) {
                clients_connected[player_id] = 1;         // Segna il client come connesso
                client_addrs[player_id] = client_addr;    // Salva l'indirizzo del client
                client_addr_lens[player_id] = addr_len;   // Salva la lunghezza dell'indirizzo
                printf("Client %d collegato\n", player_id);
            }

            // Aggiorna la posizione della racchetta in base al pacchetto ricevuto
            if (packet.type == 'P') { // Se il pacchetto è un aggiornamento della racchetta
                if (player_id == 0)
                    state.paddle_left = packet.y;  // Aggiorna la racchetta del player 0
                else
                    state.paddle_right = packet.y; // Aggiorna la racchetta del player 1
            }

            // Aggiorna la posizione della pallina in base alla logica del gioco
            update_ball(&state);

            // Stampa lo stato del gioco sul server (solo a scopo di debug)
            print_grid(&state);

            // Invia lo stato aggiornato a tutti i client connessi
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients_connected[i]) { // Verifica se il client è connesso
                    sendto(sockfd, &state, sizeof(state), 0,
                           (struct sockaddr*)&client_addrs[i], client_addr_lens[i]);
                }
            }
        }
    }

    // Chiude il socket al termine del loop (non verrà mai raggiunto in questo caso)
    close(sockfd);
}

int main() {
    server_loop(); // Avvia il loop principale del server
    return 0;      // Termina il programma (non verrà mai raggiunto)
}
