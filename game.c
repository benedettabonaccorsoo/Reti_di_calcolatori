//
// Created by Benedetta Bonaccorso on 20/10/24.
//
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
void update_ball(GameState *state) {
    state->ball_x += state->ball_dx;
    state->ball_y += state->ball_dy;

    // Collisione con le pareti verticali
    if (state->ball_y <= 0 || state->ball_y >= GRID_SIZE - 1) {
        state->ball_dy *= -1;
    }

    // Controllo punti
    if (state->ball_x < 0) {
        state->score_right++;
        reset_ball(state);
    } else if (state->ball_x >= GRID_SIZE) {
        state->score_left++;
        reset_ball(state);
    }

    // Controllo collisione con le pedine
    if (state->ball_x == 1 && state->ball_y >= state->paddle_left &&
        state->ball_y < state->paddle_left + PADDLE_SIZE) {
        state->ball_dx *= -1;
    }
    if (state->ball_x == GRID_SIZE - 2 && state->ball_y >= state->paddle_right &&
        state->ball_y < state->paddle_right + PADDLE_SIZE) {
        state->ball_dx *= -1;
    }
}

void reset_ball(GameState *state) {
    state->ball_x = GRID_SIZE / 2;
    state->ball_y = GRID_SIZE / 2;
    state->ball_dx = (rand() % 2 == 0) ? -1 : 1;
    state->ball_dy = (rand() % 2 == 0) ? -1 : 1;
}

void initialize_game(GameState *state) {
    state->ball_x = GRID_SIZE / 2;
    state->ball_y = GRID_SIZE / 2;
    state->ball_dx = -1;
    state->ball_dy = 1;
    state->paddle_left = GRID_SIZE / 2 - PADDLE_SIZE / 2;
    state->paddle_right = GRID_SIZE / 2 - PADDLE_SIZE / 2;
    state->score_left = 0;
    state->score_right = 0;
}

void print_grid(GameState* state) {
    char grid[10][10];

    // Inizializza la griglia vuota
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            grid[i][j] = ' ';
        }
    }

    // Posiziona la pallina
    grid[state->ball_y][state->ball_x] = 'O';

    // Posiziona le racchette
    for (int i = -1; i <= 1; i++) { // Racchetta sinistra
        if (state->paddle_left + i >= 0 && state->paddle_left + i < 10) {
            grid[state->paddle_left + i][0] = '|';
        }
    }

    for (int i = -1; i <= 1; i++) { // Racchetta destra
        if (state->paddle_right + i >= 0 && state->paddle_right + i < 10) {
            grid[state->paddle_right + i][9] = '|';
        }
    }

    // Stampa la griglia
    printf("\n");
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
