# Reti_di_calcolatori
gioco basato su pong usando UDP, usando il c.
Per compilare il progetto, utilizza il comando gcc:
gcc -o server server.c game.c
gcc -o client client.c game.c
Avvia il server con il comando:
./server
Avvia i client con il comando:
./client <player_id>
Dove <player id> pu essere 0 o 1.
Inserisci un numero per spostare la racchetta su o giu.
Digita exit per chiudere il client.
